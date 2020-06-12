#include "SongSystem.h"
#include "AVL.hpp"
#include "DList.hpp"
#include <stdio.h>
#include <iostream>

SongSystem::SongSystem(){
    SongCount *zero_streams = new SongCount();
    song_count_list.insert(zero_streams);
}

void SongSystem::addArtist(int artist_id, int num_of_songs) {
    if(artist_by_id.exists(artist_id)) throw FailureException();

    Artist *new_artist = new Artist();
    new_artist->id = artist_id;
    new_artist->num_of_songs = num_of_songs;
    new_artist->song_by_id = new int[num_of_songs];
    new_artist->list_pointer = new DLNode<SongCount>*[num_of_songs];

    //check if there is a zero streams node in the list making one if it doesnt exist:
    DLNode<SongCount> *lowest_streams = song_count_list.getFirst()->getNext();
    if(lowest_streams == NULL || lowest_streams->getData()->streams != 0)
    {
        SongCount *zero_streams = new SongCount();
        song_count_list.insertAfter(song_count_list.getFirst(),zero_streams);
    }

    for(int i = 0; i<num_of_songs; i++){
        new_artist->song_by_id[i] = 0;
        new_artist->list_pointer[i] = song_count_list.getFirst()->getNext();
    }

    //add artist to artist_by_id
    if(!artist_by_id.insert(new_artist,artist_id))
        throw FailureException();

    //add artist to artist_count AVL in song count list's node:
    AVL<int> *new_song_avl = new AVL<int>();
    lowest_streams = song_count_list.getFirst()->getNext();
    if(! lowest_streams->getData()->artist_count->insert(new_song_avl, artist_id)){
        //insert did not sucsed
        throw FailureException();
    }

    //add all songs to intern avl:
    new_song_avl->insertZeros(num_of_songs);

}

void SongSystem::printData() {
    std::cout << "________________"<<std::endl;
    std::cout << "artist_by_id:"<<std::endl;
    artist_by_id.printInOrder();
    std::cout << "________________"<<std::endl;
    std::cout << "song_count_list:"<<std::endl;

    DLNode<SongCount> *c = song_count_list.getFirst()->getNext();
    while(c!=NULL){
        std::cout << ">>>>artists with " <<c->getData()->streams <<" streams"<<std::endl;
        c->getData()->artist_count->printInOrder();
        c=c->getNext();
    }
    std::cout << "________________"<<std::endl;

}

void SongSystem::removeArtist(int artist_id) {
    if(!artist_by_id.exists(artist_id)) throw FailureException();

    Artist *to_remove = artist_by_id.find(artist_id);

    for(int i = 0; i<to_remove->num_of_songs; i++){
        DLNode<SongCount> *list_node = to_remove->list_pointer[i];
        AVL<int> *song_avl = list_node->getData()->artist_count->find(artist_id);
        song_avl->remove(i);

        if(song_avl->avlIsEmpty()){ //the last song of this artist in this NODe
            list_node->getData()->artist_count->remove(artist_id);
            delete song_avl;
            if(list_node->getData()->artist_count->avlIsEmpty()){
                //the artist was the last one on this node so we delete the node
                delete list_node->getData();
                song_count_list.removeNode(list_node);
            }
        }
    }

    delete[] to_remove->list_pointer;
    delete[] to_remove->song_by_id;

    artist_by_id.remove(artist_id);


    delete to_remove;
}

void SongSystem::addToSongCount(int artist_id, int song_id) {
    if(!artist_by_id.exists(artist_id)) throw FailureException();

    Artist *artist = artist_by_id.find(artist_id);
    artist->song_by_id[song_id] ++;

    DLNode<SongCount> *list_node = artist->list_pointer[song_id];
    DLNode<SongCount> *next_list_node =list_node->getNext();

    if(next_list_node == NULL ||
            (next_list_node->getData()->streams - list_node->getData()->streams) > 1){
        //next node does not exists or it has higher number of streams by more then 1
        //so we have to make new node
        SongCount *new_streams = new SongCount(list_node->getData()->streams+1);
        next_list_node = song_count_list.insertAfter(list_node, new_streams);
    }

    artist->list_pointer[song_id] = next_list_node;

    //add the artist and song to the new node:
    AVL<AVL<int>*>* next_artist_count = next_list_node->getData()->artist_count;
    if(!next_artist_count->exists(artist_id)) {
        AVL<int>* new_song_avl = new AVL<int>(song_id,song_id);
        next_artist_count->insert(new_song_avl,artist_id);
    }
    else{
        AVL<int>* song_avl_to_insert = next_artist_count->find(artist_id);
        song_avl_to_insert->insert(song_id,song_id);
    }

    //delete the song from old node, delete the artist and the node if the song was the last one
    AVL<AVL<int>*>* old_artist_count = list_node->getData()->artist_count;
    AVL<int>* old_song_avl = old_artist_count->find(artist_id);
    old_song_avl->remove(song_id);
    if(old_song_avl->avlIsEmpty()){ // the last song of this artist deleted
        old_artist_count->remove(artist_id);
        delete old_song_avl;
        if(old_artist_count->avlIsEmpty()){ //the last artist of this node deleted
            delete list_node->getData();
            song_count_list.removeNode(list_node);
        }
    }

}

int SongSystem::numberOfStreams(int artist_id, int song_id) {
    if(!artist_by_id.exists(artist_id)) throw FailureException();
    Artist *artist = artist_by_id.find(artist_id);
    return artist->song_by_id[song_id];
}

void SongSystem::getRecommendedSongs(int num_of_songs, int *artists, int *songs){
    DLNode<SongCount> *current_node = song_count_list.getLast();
    if(current_node->getData() == NULL) throw FailureException();

    int total_counter_songs = 0;
    int *artist_arr = new int[num_of_songs];
    AVL<int> **song_avl_arr = new AVL<int>*[num_of_songs];

    while(total_counter_songs < num_of_songs){
        int num_of_artists_in_node = current_node->getData()->artist_count->
                getKInOrder(num_of_songs,song_avl_arr, artist_arr);

        for(int i=0; i<num_of_artists_in_node; i++){
            int songs_left = num_of_songs - total_counter_songs;
            int *arr_not_used = new int[songs_left];
            int counter_songs = song_avl_arr[i]->getKInOrder(songs_left,
                                         songs+total_counter_songs,arr_not_used);
            for(int k=0; k<counter_songs; k++) artists[total_counter_songs+k] = artist_arr[i];

            total_counter_songs+= counter_songs;

            if(total_counter_songs >= num_of_songs){
                delete[] artist_arr;
                delete[] song_avl_arr;
                delete[] arr_not_used;
                return;
            }
            delete[] arr_not_used;
        }
        current_node = current_node->getPrev();
        if(current_node ->getData() == NULL){
            delete[] artist_arr;
            delete[] song_avl_arr;
            throw FailureException();
        }
    }
   // std::cout<< " GetRecommendedSongs - we should not get here! " << std::endl;
}

SongSystem::~SongSystem() {
    //free data in the list:
    DLNode<SongCount> *current_node = song_count_list.getLast();
    DLNode<SongCount> *temp;
    while(current_node->getData() != NULL){
        free_node_data(current_node);
        temp =current_node->getPrev();
        song_count_list.removeNode(current_node);
        current_node = temp;
    }

    //free data in array:
    int num_of_artists = artist_by_id.getNodesCount();
    Artist** artist_p_array = new Artist*[num_of_artists];
    int *not_used_array = new int[num_of_artists];
    artist_by_id.getKInOrder(num_of_artists, artist_p_array, not_used_array);
    for(int i=0; i<num_of_artists; i++){
        delete[] artist_p_array[i]->song_by_id;
        delete[] artist_p_array[i]->list_pointer;
        delete artist_p_array[i];
    }

    delete[] artist_p_array;
    delete[] not_used_array;

    //list and array automaticly freed
}

void SongSystem::free_node_data(DLNode<SongCount> *pNode) {
    AVL<AVL<int>*>* ac_avl =  pNode->getData()->artist_count;
    int ac_avl_size = ac_avl->getNodesCount();
    AVL<int>** song_avl_arr = new AVL<int>*[ac_avl_size];
    int *not_used_array = new int[ac_avl_size];
    ac_avl->getKInOrder(ac_avl_size, song_avl_arr, not_used_array);

    for(int i=0; i<ac_avl_size; i++){
        delete song_avl_arr[i];
    }
    delete pNode->getData();
    delete[] not_used_array;
    delete[] song_avl_arr;
}

int SongSystem::numberOfSongs(int artist_id) {
    if(!artist_by_id.exists(artist_id)) throw FailureException();
    Artist *artist = artist_by_id.find(artist_id);
    return artist->num_of_songs;
};

