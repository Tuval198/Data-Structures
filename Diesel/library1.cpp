#include "library1.h"
#include "SongSystem.h"

void* Init(){
    try{
        SongSystem* DS = new SongSystem();
        return (void*)DS;
    }
    catch(std::bad_alloc& ba){
        return NULL;
    }
}

StatusType AddArtist(void *DS, int artistID, int numOfSongs) {
    if(numOfSongs<=0 || DS==NULL || artistID<=0 )
        return INVALID_INPUT;

    try{
        SongSystem* ss = (SongSystem*)DS;
        ss->addArtist(artistID,numOfSongs);
    }
    catch (std::bad_alloc& ba){
        return ALLOCATION_ERROR;
    }
    catch(SongSystem::FailureException&){
        return FAILURE;
    }

    return SUCCESS;
}

StatusType RemoveArtist(void *DS, int artistID) {
    if(DS == NULL || artistID <= 0)
        return INVALID_INPUT;

    try{
        SongSystem* ss = (SongSystem*)DS;
        ss->removeArtist(artistID);
    }
    catch (std::bad_alloc& ba){
        return ALLOCATION_ERROR;
    }
    catch(SongSystem::FailureException&){
        return FAILURE;
    }
    return SUCCESS;
}

StatusType AddToSongCount(void *DS, int artistID, int songID) {
    if(DS == NULL || artistID <=0 || songID <0)
        return  INVALID_INPUT;
    try{
        SongSystem* ss = (SongSystem*)DS;
        if(ss->numberOfSongs(artistID) <= songID)
            return  INVALID_INPUT;
        ss->addToSongCount(artistID,songID);
    }
    catch (std::bad_alloc& ba){
        return ALLOCATION_ERROR;
    }
    catch(SongSystem::FailureException&){
        return FAILURE;
    }
    return SUCCESS;

}

StatusType NumberOfStreams(void *DS, int artistID, int songID, int *streams) {
    if(DS == NULL || artistID <=0 || songID <0 || streams == NULL)
        return  INVALID_INPUT;
    try{
        SongSystem* ss = (SongSystem*)DS;
        if(ss->numberOfSongs(artistID) <= songID)
            return  INVALID_INPUT;
        *streams = ss->numberOfStreams(artistID,songID);
    }
    catch (std::bad_alloc& ba){
        return ALLOCATION_ERROR;
    }
    catch(SongSystem::FailureException&){
        return FAILURE;
    }
    return SUCCESS;
}

StatusType GetRecommendedSongs(void *DS, int numOfSongs, int *artists, int *songs) {
    if(DS == NULL)
        return INVALID_INPUT;
    if(numOfSongs <= 0)
        return ALLOCATION_ERROR;
    try{
        SongSystem* ss = (SongSystem*)DS;
        ss->getRecommendedSongs(numOfSongs,artists,songs);
    }
    catch (std::bad_alloc& ba){
        return ALLOCATION_ERROR;
    }
    catch(SongSystem::FailureException&){
        return FAILURE;
    }
    return SUCCESS;
}

void Quit(void **DS) {
    SongSystem* ss = (SongSystem*)*DS;
    delete ss;
    *DS = NULL;
}







