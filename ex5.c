/***********
 ID:
 NAME:
***********/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_LENGTH_FOR_EPISODE 8
#define PLACE_OF_COLON 3
#define BASE_10_LAST_DIGIT '9'
#define BASE_10_FIRST_DIGIT '0'
#define MAX_TENS_DIGIT_IN_TIME '6'
#define TRUE 1
#define FALSE 0

typedef struct Episode {
    char *name;
    char *length;
    struct Episode *next;
} Episode;

typedef struct Season {
    char *name;
    Episode *episodes;
    struct Season *next;
} Season;

typedef struct TVShow {
    char *name;
    Season *seasons;
} TVShow;

TVShow ***database = NULL;
int dbSize = 0;

// functions signetures

void removeSpacesFromString(char*);
char *getString();
int validLength(char*);
int countShows();

// resize functions signatures
void shrinkDB();
void expandDB();

// free functions signatures
void freeEpisodes(Episode*);
void freeSeasons(Season*);
void freeShow(TVShow*);
void freeAll();

// find functions signatures
TVShow *findShow(char *);
Season *findSeason(TVShow *, char *);
Episode *findEpisode(Season *, char *);

// add and delete functions signatures
void addEpisodeInCorrectPlace(Season *, char *, char *, int);
void addSeasonInCorrectPlace(TVShow *, char *, int);
void addShowInCorrectPlace(char*);
void addShow();
void addSeason();
void addEpisode();
void deleteShow();
void deleteSeason();
void deleteEpisode();

// print functions signatures
void printEpisode();
void printSeason();
void printShow();

// functions implemen
void removeSpacesFromString(char *str) {
    if (str == NULL){
        return;
    }
    
    int len = strlen(str);
    
    // delete all the spaces at the end of a string (like "hi hi   " will be "hi hi") 
    while (len > 0 && (str[len - 1] == ' ' || 
        str[len - 1] == '\t' || str[len - 1] == '\r')) {
        // replace space with \0
        str[len - 1] = '\0';
        len--;
    }
}
char *getString(){
    char bufferChar;
    char *str=(char*)malloc(sizeof(char));
    if(str==NULL){
        //Memory allocation failed
        exit(1);
    }
    scanf(" %c",str);
    bufferChar=getchar();
    int length = 1;
    while(bufferChar!='\n'){
        
        str=(char*)realloc(str, sizeof(char)*length+1);
        if(str==NULL){
            //Memory allocation failed
            exit(1);
        }

        /* append the new character to the string
        use length as the index for the next character because length is 
        the current size of the string before adding the new character and 
        we start counting from 0 so the next index is equal to length*/
        *(str+length)=bufferChar;

        // get the next character
        bufferChar=getchar();

        // increment the length of the string
        length++;
    }
    if(str!=NULL){
        *(str+length)='\0'; // null-terminate the string
        // remove spaces from the end of the string
        removeSpacesFromString(str);
        
        return str;
    }
    return NULL;
}

int validLength(char *s){
    // check if the length of the string is MAX_LENGTH_FOR_EPISODE
    if((int)strlen(s) != MAX_LENGTH_FOR_EPISODE){
        return FALSE;
    }

    // check each character in the string
    for(int i=0; i < (int)strlen(s); i++){

        // check if thre is a colon in the correct place
        if(!((i + 1) % PLACE_OF_COLON)){
            if(s[i]!=':'){
                return FALSE;
            }
        }
        else{
            // check if the character is a digit
            if(!(s[i] >= BASE_10_FIRST_DIGIT && s[i] <= BASE_10_LAST_DIGIT)){
                return FALSE;
            }
            // check if the tens digit in time is valid (less than 6)
            if((i + 1) % PLACE_OF_COLON == 1 && i != 0){
                if(s[i] >= MAX_TENS_DIGIT_IN_TIME){
                    return FALSE;
                }
            }
        }
    }
    // if all checks passed return 1
    return TRUE;
}
int countShows(){
    int sumOfExistingShows = 0;
    for(int i = 0; i < dbSize; i++){
        for(int j = 0; j < dbSize; j++){
            if(database[i][j]){
                sumOfExistingShows++;
            }
        }
    }
    return sumOfExistingShows;
}

void shrinkDB(){
    
    // free the last row (to the last column we will do realloc later - to each row)
    free(database[dbSize - 1]);

    dbSize--; // decreasing dimension by 1

    // if dbSize is 0 after decreasing free the database and set it to NULL
    if (dbSize == 0) {
        free(database);
        database = NULL;
        return;
    }

    // decreasing the row cells
    database = realloc(database, sizeof(TVShow **) * dbSize);
    
    // check if realloc was successful
    if (database == NULL) {
        exit(1);
    }

    /* shrink all existing cells (from the old size) in the new row 
    (realloc columns in the new size to them)*/
    for (int i = 0; i < dbSize; i++) {
        database[i] = realloc(database[i], sizeof(TVShow *) * dbSize);
        if (database[i] == NULL) {
            exit(1);
        }
    }
}

void expandDB(){
    int oldSize = dbSize;
    dbSize++; // incresing dimension by 1

    // increasing the row cells
    database = (TVShow ***)realloc(database, sizeof(TVShow **) * dbSize);
    if (database == NULL) {
        exit(1);
    }

    /* expand all existing cells (from the old size) in the new row
    (realloc columns in the new size to them)*/
    for (int i = 0; i < oldSize; i++) {
        database[i] = (TVShow **)realloc(database[i], sizeof(TVShow *) * dbSize);
        if (database[i] == NULL) {
            exit(1);
        }
        
        // initialize to NULL the new cell at the end of each existing row
        database[i][dbSize - 1] = NULL;
    }

    // create the last new row (the last floor of the matrix)
    database[dbSize - 1] = (TVShow **)malloc(sizeof(TVShow *) * dbSize);
    if (database[dbSize - 1] == NULL) {
        exit(1);
    }
    
    // initialize to NULL all the cells in the new row
    for (int j = 0; j < dbSize; j++) {
        database[dbSize - 1][j] = NULL;
    }
}

// free all the episodes in the linked list - need to get the head of the episodes linked list
void freeEpisodes(Episode *e){
    Episode *currentEpisode = e;
    Episode *previousEpisode = e;

    // until the end of the episodes linked list
    while(currentEpisode){
        currentEpisode = previousEpisode -> next;
        free(previousEpisode -> name);
        free(previousEpisode -> length);
        free(previousEpisode);
        previousEpisode = currentEpisode;
    }
}

// free all the seasons in the linked list - need to get the head of the seasons linked list
void freeSeasons(Season *s){
    Season *currentSeason = s;
    Season *previousSeason = s;

    // until the end of the seasons linked list
    while(currentSeason){
        currentSeason = previousSeason -> next;
        freeEpisodes(previousSeason -> episodes);
        free(previousSeason -> name);
        free(previousSeason);
        previousSeason = currentSeason;
    }
}
void freeShow(TVShow *show){
    freeSeasons(show -> seasons);
    free(show -> name);
    free(show);
}
void freeAll(){
    for(int i = 0; i < dbSize; i++){
        for(int j = 0; j < dbSize; j++){
            // check if the cell is not empty
            if(database[i][j]){
            
                // free the show
                freeShow(database[i][j]);
            }
        }
    }
}

TVShow *findShow(char *name){
    TVShow *show = NULL;
    for(int i = 0; i < dbSize; i++){
        for(int j = 0; j < dbSize; j++){
            // check if the cell is not empty and the name matches
            if(database[i][j] && strcmp(database[i][j] -> name,name) == 0){
                show = database[i][j];
                break;
            }
        }
    }
    return show;
}

Season *findSeason(TVShow *show, char *name){
    Season *pointerToSeason = show -> seasons;
    
    //check each season in the linked list
    while(pointerToSeason){

        // check if the season name matches
        if(strcmp(pointerToSeason -> name, name) == 0){
            return pointerToSeason;
        }
        
        // move to the next season
        pointerToSeason = pointerToSeason -> next;
    }
    return NULL;
}

Episode *findEpisode(Season *season, char *name){
    
    Episode *currentEpisode = season -> episodes;
    
    // continue until the end of the episodes linked list or until we find the episode
    while(currentEpisode){
        
        // check if the episode name matches
        if(strcmp(( currentEpisode -> name), name) == 0){
            return currentEpisode;
        }
        // move to the next episode
        currentEpisode = currentEpisode -> next;
    }
    return NULL;
}

void addShow(){
    // get the show name
    printf("Enter the name of the show:\n");
    char *currentTVName = getString();
    if(currentTVName == NULL){
        exit(1);
    }

    // check if the show already exists
    if(findShow(currentTVName) != NULL){
        free(currentTVName);
        printf("Show already exists.\n");
    }
    // if the show does not exist
    else{
        // if there are no existing shows
        if(database == NULL || dbSize == 0){
            // initialize the database
            dbSize++;
            database = (TVShow ***)malloc(sizeof(TVShow **)*dbSize);
            if(database == NULL){
                exit(1);
            }
            // create the first row
            database[0] = (TVShow **)malloc(sizeof(TVShow *)*dbSize);
            if(database[0] == NULL){
                exit(1);
            }

            // create the new show
            TVShow *newShow = (TVShow *)malloc(sizeof(TVShow));
            if(newShow == NULL){
                exit(1);
            }
            // add the new show
            newShow -> name = currentTVName;
            newShow -> seasons = NULL;            
            database[0][0] = newShow;
        }
        // if there are existing shows
        else{
            int howManyShowsExist = countShows();
            // check if we need to expand the database
            if(howManyShowsExist == dbSize * dbSize){
                expandDB();
                addShowInCorrectPlace(currentTVName);
            }
            // if we dont need to expand the database
            else{
                addShowInCorrectPlace(currentTVName);
            }
        }
    }
}

void addSeasonInCorrectPlace(TVShow *tvShow, char *seasonName, int position){
    // if there are no existing seasons
    if(!(tvShow -> seasons)){

        // create first season
        Season *head = (Season *)malloc(sizeof(Season));
        if(head == NULL){
            exit(1);
        }

        // add the new season
        head -> name = seasonName;
        head -> episodes = NULL;
        head -> next = NULL;
        tvShow -> seasons = head;
    }
    // if there are existing seasons
    else{
        //find the correct place to add the season
        if(position != 0){
            int countUntilPos = 0;
            Season *pointerToSeason = tvShow -> seasons;
            if(countUntilPos + 1 < position){
                while(pointerToSeason -> next){
                    if(countUntilPos + 1 >= position){
                        break;
                    }
                    pointerToSeason = pointerToSeason -> next;
                    countUntilPos++;
                }
            }

            // create new season
            Season *newSeason = (Season *)malloc(sizeof(Season));
            if(newSeason == NULL){
                exit(1);
            }

            // add the new season in the current place
            newSeason -> name = seasonName;
            newSeason -> episodes = NULL;
            newSeason -> next = pointerToSeason -> next;
            
            pointerToSeason -> next = newSeason;
        }
        else{
            // create new season
            Season *newSeason = (Season *)malloc(sizeof(Season));
            if(newSeason == NULL){
                exit(1);
            }

            // add the new season at the begining
            newSeason -> name = seasonName;
            newSeason -> episodes = NULL;
            newSeason -> next = tvShow -> seasons;
            
            tvShow -> seasons = newSeason;
        }
    }
}

void addShowInCorrectPlace(char *nameCurrentChar){
    
    // covert the array to one dimensional for easier sorting
    TVShow **tempArray = (TVShow **)malloc(sizeof(TVShow *) * (dbSize * dbSize)); 
    
    if(tempArray == NULL){
        exit(1);
    }

    // copy all the cells value from database to the new array
    int cellNum = 0;
    for(int i = 0; i < dbSize; i++){
        for(int j = 0; j < dbSize; j++){
            tempArray[cellNum] = database[i][j];
            cellNum++;
        }
    }

    int emptyPlace = -1;
    int firstNameAfterchosenNamePlace = -1;
    int dbNewLength = dbSize * dbSize;

    // find the empty place and the first name after the chosen name in the alphabet
    for(int i = 0; i < dbNewLength; i++){
    
        // check if the cell is empty
        if(tempArray[i] == NULL){
            // store the first empty place found
            if (emptyPlace == -1) {
                emptyPlace = i;
            }
            // if we found both values we can stop searching
            break; 
        }
        else{
            // check if the name in the cell is after the chosen name in the alphabet
            if(firstNameAfterchosenNamePlace == -1 && 
                strcmp(nameCurrentChar, tempArray[i] -> name) < 0){
                firstNameAfterchosenNamePlace = i;
            }
        }
    }

    // create the new show
    TVShow *newShow = (TVShow *)malloc(sizeof(TVShow));
    if(newShow == NULL){
        free(tempArray); // Don't forget to free tempArray before exiting
        exit(1);
    }

    newShow -> name = nameCurrentChar;
    newShow -> seasons = NULL;

    // put the new show in the correct place
    if(firstNameAfterchosenNamePlace != -1){
        
        // the new show is alphabetically before at least one existing show
        
        // shift all the shows from firstNameAfterchosenNamePlace to emptyPlace to the right
        for(int i = emptyPlace; i > firstNameAfterchosenNamePlace; i--){
            tempArray[i] = tempArray[i-1];
        }

        // Place the new show in the created gap
        tempArray[firstNameAfterchosenNamePlace] = newShow;
    }
    else{
        // the new show is alphabetically after all existing shows
        
        // place the new show in the first empty place found
        if(emptyPlace != -1){
            tempArray[emptyPlace] = newShow;
        }
    }

    // returning all the values from the temp array to the database
    int index = 0;
    for(int i = 0; i < dbSize; i++){
        for(int j = 0; j < dbSize; j++){
            database[i][j] = tempArray[index];
            index++;
        }
    }

    free(tempArray); // free the temp array
}

void addEpisodeInCorrectPlace(Season *season,
        char *episodeName, char *episodeLength, int position){
    
    // if there are no existing episodes
    if(!(season -> episodes)){
        // create first episode
        Episode *head = (Episode *)malloc(sizeof(Episode));
        if(head == NULL){
            exit(1);
        }

        // add the new episode
        head -> name = episodeName;
        head -> length = episodeLength;
        head -> next = NULL;
        season -> episodes = head;
    }
    // if there are existing episodes
    else{
        //find the correct place to add the episode
        Episode *pointerToEpisode = season -> episodes;
        if(position > 0){
            int countUntilPos = 0;
            if(countUntilPos + 1 < position){
                while(pointerToEpisode -> next){
                    if(countUntilPos + 1 >= position){
                        break;
                    }
                    pointerToEpisode = pointerToEpisode -> next;
                    countUntilPos++;
                }
            }

            // create new episode
            Episode *newEpisode = (Episode *)malloc(sizeof(Episode));
            if(newEpisode == NULL){
                exit(1);
            }

            // add the new episode in the current place
            newEpisode -> name = episodeName;
            newEpisode -> length = episodeLength;
            newEpisode -> next = pointerToEpisode -> next;
            pointerToEpisode -> next = newEpisode;
        }
        else{
            // create new episode
            Episode *newEpisode = (Episode *)malloc(sizeof(Episode));
            if(newEpisode == NULL){
                exit(1);
            }

            // add the new episode at the begining
            newEpisode -> name = episodeName;
            newEpisode -> length = episodeLength;
            newEpisode -> next = pointerToEpisode;
            season -> episodes = newEpisode;
        }
    }
}

void addSeason(){
    // get the show name
    printf("Enter the name of the show:\n");
    char *chosenTVShowName = getString();
    if(chosenTVShowName == NULL){
        exit(1);
    }

    // find the show in the database
    TVShow *currentTVShow = findShow(chosenTVShowName);
    if(!currentTVShow){
        printf("Show not found.\n");
        free(chosenTVShowName);
        return;
    }

    free(chosenTVShowName); // not in use anymore

    // get the season name
    printf("Enter the name of the season:\n");
    char *chosenTVShowSeason = getString();
    if(chosenTVShowSeason == NULL){
        exit(1);
    }

    // check if the season already exists
    if(findSeason(currentTVShow, chosenTVShowSeason)){
        printf("Season already exists.\n");
        free(chosenTVShowSeason);
        return;
    }
    printf("Enter the position:\n");
    
    // get the position
    int position = -1;
    scanf(" %d", &position);
    
    // clean buffer
    int currentChar;
    while ((currentChar = getchar()) != '\n' && currentChar != EOF);
    
    // add the season in the correct place
    if(position >= 0){
        addSeasonInCorrectPlace(currentTVShow, chosenTVShowSeason, position);
    }
}

void addEpisode(){
    // get the show name
    printf("Enter the name of the show:\n");
    char *chosenTVShowName = getString();
    if(chosenTVShowName == NULL){
        exit(1);
    }

    // find the show in the database
    TVShow *currentTVShow = findShow(chosenTVShowName);
    if(!currentTVShow){
        printf("Show not found.\n");
        free(chosenTVShowName);
        return;
    }

    free(chosenTVShowName); // not in use anymore

    // get the season name
    printf("Enter the name of the season:\n");
    char *chosenTVShowSeasonName = getString();
    if(chosenTVShowSeasonName == NULL){
        exit(1);
    }

    // find the season in the show
    Season *chosenTVShowSeason = findSeason(currentTVShow, chosenTVShowSeasonName);
    if(!chosenTVShowSeason){
        printf("Season not found.\n");
        free(chosenTVShowSeasonName);
        return;
    }

    free(chosenTVShowSeasonName); // not in use anymore

    // get the episode name
    printf("Enter the name of the episode:\n");
    char *chosenEpisodeName = getString();
    if(chosenEpisodeName == NULL){
        exit(1);
    }

    // check if the episode already exists
    if(findEpisode(chosenTVShowSeason, chosenEpisodeName)){
        printf("Episode already exists.\n");
        free(chosenEpisodeName);
        return;
    }

    // get the episode length
    printf("Enter the length (xx:xx:xx):\n");
    char *chosenEpisodeLength = getString();
    if(chosenEpisodeLength == NULL){
        exit(1);
    }
    
    // validate the length and ask again if not valid
    while(!validLength(chosenEpisodeLength)){
        printf("Invalid length, enter again:\n");
        free(chosenEpisodeLength);
        chosenEpisodeLength = getString();
        if(chosenEpisodeLength == NULL){
            exit(1);
        }
    }

    // get the position
    printf("Enter the position:\n");
    int position = -1;
    scanf(" %d", &position);
    
    // clean buffer
    int currentChar;
    while ((currentChar = getchar()) != '\n' && currentChar != EOF);
    
    // add the episode in the correct place
    if(position >= 0){
        addEpisodeInCorrectPlace(chosenTVShowSeason,
        chosenEpisodeName, chosenEpisodeLength, position);
    }
}

void deleteShow(){
    // get the show name to delete
    printf("Enter the name of the show:\n");
    char *chosenTVShowName = getString();
    if(chosenTVShowName == NULL){
        exit(1);
    }

    // find the show in the database
    TVShow *currentTVShow = findShow(chosenTVShowName);
    if(!currentTVShow){
        printf("Show not found.\n");
        free(chosenTVShowName);
        return;
    }
    
    free(chosenTVShowName); // not in use anymore
    
    int row;
    int col;

    // find the show in the database and set it to NULL
    for(row = 0; row < dbSize; row++){
        for(col = 0; col < dbSize; col++){
            if(database[row][col] && database[row][col] == currentTVShow){
                database[row][col] = NULL;
                break;
            }
        }
    }

    // covert the array to one dimensional for easier sorting
    TVShow **tempArray = (TVShow **)malloc(sizeof(TVShow *) * (dbSize * dbSize)); 
    
    if(tempArray == NULL){
        exit(1);
    }

    // copy all the cells value from database to the new array
    int cellNum = 0;
    for(int i = 0; i < dbSize; i++){
        for(int j = 0; j < dbSize; j++){
            if(database[i][j]){
                tempArray[cellNum] = database[i][j];
                cellNum++;
            }
        }
    }

    // fill the rest of the temp array with NULLs
    for(int i = cellNum; i < dbSize * dbSize; i++){
        tempArray[i] = NULL;
    }
    
    // returning all the values from the temp array to the database
    int index = 0;
    for(int i = 0; i < dbSize; i++){
        for(int j = 0; j < dbSize; j++){
            database[i][j] = tempArray[index];
            index++;
        }
    }

    free(tempArray); // free the temp array

    int countExsitingShows = countShows();
    int sumOfCellsWithoutLastRowAndCol = (dbSize - 1) * (dbSize - 1);
    
    /* check if we need to shrink the database
    the condition is if the number of cells without the last row and column
    is equal to the exsiting number of shows in the database 
    because if we remove the last row and column all the cells will be empty */
    if(countExsitingShows == sumOfCellsWithoutLastRowAndCol){
        shrinkDB();
    }

    // free the show
    freeShow(currentTVShow);
    
}

void deleteSeason(){
    // get the show name
    printf("Enter the name of the show:\n");
    char *chosenTVShowName = getString();
    if(chosenTVShowName == NULL){
        exit(1);
    }

    // find the show in the database
    TVShow *currentTVShow = findShow(chosenTVShowName);
    if(!currentTVShow){
        printf("Show not found.\n");
        free(chosenTVShowName);
        return;
    }
    
    free(chosenTVShowName); // not in use anymore

    // get the season name to delete
    printf("Enter the name of the season:\n");
    char *chosenTVShowSeasonName = getString();
    if(chosenTVShowSeasonName == NULL){
        exit(1);
    }

    // find the season in the show
    Season *chosenTVShowSeason = findSeason(currentTVShow, chosenTVShowSeasonName);
    if(!chosenTVShowSeason){
        printf("Season not found.\n");
        free(chosenTVShowSeasonName);
        return;
    }
    free(chosenTVShowSeasonName); // not in use anymore
    
    // remove the season from the show's seasons linked list
    if(currentTVShow -> seasons != chosenTVShowSeason){
        Season *precedingSeason = currentTVShow -> seasons;
        while(precedingSeason -> next != chosenTVShowSeason){
            precedingSeason = precedingSeason -> next;
        }
        precedingSeason -> next = chosenTVShowSeason -> next;
    }
    // if the season to delete is the first season
    else{
        currentTVShow -> seasons = chosenTVShowSeason -> next;
    }

    // free the season
    freeEpisodes(chosenTVShowSeason -> episodes);
    free(chosenTVShowSeason -> name);
    free(chosenTVShowSeason);
}
void deleteEpisode(){
    // get the show name
    printf("Enter the name of the show:\n");
    char *chosenTVShowName = getString();
    if(chosenTVShowName == NULL){
        exit(1);
    }

    // find the show in the database
    TVShow *currentTVShow = findShow(chosenTVShowName);
    if(!currentTVShow){
        printf("Show not found.\n");
        free(chosenTVShowName);
        return;
    }

    free(chosenTVShowName); // not in use anymore

    // get the season name
    printf("Enter the name of the season:\n");
    char *chosenTVShowSeasonName = getString();
    if(chosenTVShowSeasonName == NULL){
        exit(1);
    }

    // find the season in the show
    Season *chosenTVShowSeason = findSeason(currentTVShow, chosenTVShowSeasonName);
    if(!chosenTVShowSeason){
        printf("Season not found.\n");
        free(chosenTVShowSeasonName);
        return;
    }

    free(chosenTVShowSeasonName); //not in use anymore

    // get the episode name to delete
    printf("Enter the name of the episode:\n");
    char *chosenEpisodeName = getString();
    if(chosenEpisodeName == NULL){
        exit(1);
    }

    // find the episode in the season
    Episode *currentEpisode = findEpisode(chosenTVShowSeason, chosenEpisodeName);
    if(!currentEpisode){
        printf("Episode not found.\n");
        free(chosenEpisodeName);
        return;
    }

    free(chosenEpisodeName); // not in use anymore
    
    // remove the episode from the season's episodes linked list
    Episode *previousEpisode = chosenTVShowSeason -> episodes;
    if(previousEpisode != currentEpisode){
        while(previousEpisode -> next != currentEpisode){
            previousEpisode = previousEpisode -> next;
        }
        previousEpisode -> next = currentEpisode -> next;
    }
    // if the episode to delete is the first episode
    else{
        chosenTVShowSeason -> episodes = currentEpisode -> next;
    }
    
    // free the episode
    free(currentEpisode -> name);
    free(currentEpisode -> length);
    free(currentEpisode);
}

void printEpisode(){
    // get the show name
    printf("Enter the name of the show:\n");
    char *chosenTVShowName = getString();
    if(chosenTVShowName == NULL){
        exit(1);
    }

    // find the show in the database
    TVShow *currentTVShow = findShow(chosenTVShowName);
    if(!currentTVShow){
        printf("Show not found.\n");
        free(chosenTVShowName);
        return;
    }

    free(chosenTVShowName); // not in use anymore

    // get the season name
    printf("Enter the name of the season:\n");
    char *chosenTVShowSeasonName = getString();
    if(chosenTVShowSeasonName == NULL){
        exit(1);
    }

    // find the season in the show
    Season *chosenTVShowSeason = findSeason(currentTVShow, chosenTVShowSeasonName);
    if(!chosenTVShowSeason){
        printf("Season not found.\n");
        free(chosenTVShowSeasonName);
        return;
    }

    free(chosenTVShowSeasonName); //not in use anymore

    // get the episode name
    printf("Enter the name of the episode:\n");
    char *chosenEpisodeName = getString();
    if(chosenEpisodeName == NULL){
        exit(1);
    }

    // find the episode in the season
    Episode *currentEpisode = findEpisode(chosenTVShowSeason, chosenEpisodeName);
    if(!currentEpisode){
        printf("Episode not found.\n");
        free(chosenEpisodeName);
        return;
    }

    free(chosenEpisodeName); // not in use anymore
    // print the episode details
    printf("Name: %s\nLength: %s\n", currentEpisode -> name, currentEpisode -> length);
}

void printShow(){
    // get the show name
    printf("Enter the name of the show:\n");
    char *chosenTVShowName = getString();
    if(chosenTVShowName == NULL){
        exit(1);
    }

    // find the show in the database
    TVShow *currentTVShow = findShow(chosenTVShowName);
    if(!currentTVShow){
        printf("Show not found.\n");
        free(chosenTVShowName);
        return;
    }

    // print the show details
    printf("Name: %s\n", chosenTVShowName);
    printf("Seasons:\n");
    Season *currentSeason = currentTVShow -> seasons;
    int seasonCount = 0;
    int episodeCount = 0;
    while(currentSeason){
        printf("\tSeason %d: %s\n", seasonCount, currentSeason -> name);
        Episode *currentEpisode = currentSeason -> episodes;
        while(currentEpisode){
            printf("\t\tEpisode %d: %s (%s)\n", episodeCount, 
                currentEpisode -> name, currentEpisode -> length);
            currentEpisode = currentEpisode -> next;
            episodeCount++;
        }
        currentSeason = currentSeason -> next;
        seasonCount++;
    }

    free(chosenTVShowName); // not in use anymore
}
void printArray(){
    for(int i = 0; i < dbSize; i++){
        for(int j = 0; j < dbSize; j++){
            if(database[i][j]){
                printf("[%s] ", database[i][j] -> name);
            }
            else{
                printf("[NULL] ");
            }
        }
        printf("\n");
    }
}

void addMenu() {
    int choice;
    printf("Choose an option:\n");
    printf("1. Add a TV show\n");
    printf("2. Add a season\n");
    printf("3. Add an episode\n");
    scanf("%d", &choice);
    getchar();
    switch (choice) {
        case 1: addShow(); break;
        case 2: addSeason(); break;
        case 3: addEpisode(); break;
    }
}

void deleteMenu() {
    int choice;
    printf("Choose an option:\n");
    printf("1. Delete a TV show\n");
    printf("2. Delete a season\n");
    printf("3. Delete an episode\n");
    scanf("%d", &choice);
    getchar();
    switch (choice) {
        case 1: deleteShow(); break;
        case 2: deleteSeason(); break;
        case 3: deleteEpisode(); break;
    }
}

void printMenuSub() {
    int choice;
    printf("Choose an option:\n");
    printf("1. Print a TV show\n");
    printf("2. Print an episode\n");
    printf("3. Print the array\n");
    scanf("%d", &choice);
    getchar();
    switch (choice) {
        case 1: printShow(); break;
        case 2: printEpisode(); break;
        case 3: printArray(); break;
    }
}

void mainMenu() {
    printf("Choose an option:\n");
    printf("1. Add\n");
    printf("2. Delete\n");
    printf("3. Print\n");
    printf("4. Exit\n");
}

int main() {
    int choice;
    do {
        mainMenu();
        scanf("%d", &choice);
        getchar();
        switch (choice) {
            case 1: addMenu(); break;
            case 2: deleteMenu(); break;
            case 3: printMenuSub(); break;
            case 4: freeAll(); break;
        }
    } while (choice != 4);
    return 0;
}
