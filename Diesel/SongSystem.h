#ifndef WET1_SONGSYSTEM_H
#define WET1_SONGSYSTEM_H

#include "DList.hpp"
#include "AVL.hpp"
#include <stdio.h>

struct SongCount{
    int streams;
    AVL<AVL<int>*>* artist_count;

    SongCount(int streams = 0): streams(streams),artist_count(new AVL<AVL<int>*>){};
    ~SongCount(){delete artist_count;}
};


struct Artist{
    int id;
    int num_of_songs;
    int *song_by_id;
    DLNode<SongCount> **list_pointer ;
};

class SongSystem{
private:
    AVL<Artist*> artist_by_id;
    DList<SongCount> song_count_list;

    void free_node_data(DLNode<SongCount> *pNode);
public:
    SongSystem();
    ~SongSystem();
    void addArtist(int artist_id, int num_of_songs);
    void removeArtist(int artist_id);
    void addToSongCount(int artist_id, int song_id);
    int numberOfStreams(int artist_id, int song_id);
    void getRecommendedSongs(int num_of_songs, int *artists, int *songs);

    int numberOfSongs(int artist_id);
    void printData();
    class FailureException {};
};

#endif //WET1_SONGSYSTEM_H
