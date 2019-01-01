/*
    C socket server example, handles multiple clients using threads
    Compile
    gcc server.c -lpthread -o server
*/
 
#include<stdio.h>
#include<string.h>    //strlen
#include<stdlib.h>    //strlen
#include<sys/socket.h>
#include<arpa/inet.h> //inet_addr
#include<unistd.h>    //write
#include<pthread.h> //for threading , link with lpthread
////File desc for pipe
//int fd[2];
////////////////////////////

pthread_mutex_t count_mutex;
pthread_mutex_t punkty_mutex;
pthread_mutex_t id_mutex;
long long count = 0;
int tablica_punktow[100];
int id_gracza = 0;

int
get_id_gracza_and_inc()
{
    pthread_mutex_lock(&id_mutex);
    id_gracza = id_gracza + 1;
    pthread_mutex_unlock(&id_mutex); 
    return id_gracza;
}

int
get_id_gracza()
{
    int id;
    pthread_mutex_lock(&id_mutex);
    id = id_gracza;
    pthread_mutex_unlock(&id_mutex); 
    return id;
}

void
increment_count()
{
    pthread_mutex_lock(&count_mutex);
    count = count + 1;
    pthread_mutex_unlock(&count_mutex);
}


void
decrement_count()
{
    pthread_mutex_lock(&count_mutex);
    count = count - 1;
    pthread_mutex_unlock(&count_mutex);
}

long long
get_count()
{
    long long c;
    
    pthread_mutex_lock(&count_mutex);
    c = count;
    pthread_mutex_unlock(&count_mutex);
    return (c);
}
void
dodaj_swoje_punkty(int punkty,int numer_gracza)
{
    pthread_mutex_lock(&punkty_mutex);
    tablica_punktow[numer_gracza] = punkty;
    printf("Gracz o ID: %d dodał punkty: %d\n",numer_gracza,punkty);
    fflush(stdout);    
    pthread_mutex_unlock(&punkty_mutex);
}
int czy_wygrana(int numer_gracza,int punkty)
{puts("Czy wygrana?");
    pthread_mutex_lock(&punkty_mutex);
    //int k=0,l=0;
    //for(k = 0;k<id_gracza;k++){
    //    for(l = 0;l<id_gracza;l++){
    //        if(tablica_punktow[l]<tablica_punktow[k])
    //        {
    //            int temp = tablica_punktow[k];
    //            tablica_punktow[k] = tablica_punktow[l];
    //            tablica_punktow[l] = temp;
    //        }
    //    }
    //}
    int maximum = tablica_punktow[0];
    int location = 0;
    int c;
    for (c = 1; c < id_gracza; c++)
    {
        if (tablica_punktow[c] > maximum)
        {
            maximum  = tablica_punktow[c];
            location = c+1;
        }
    }
    //Wyświetlanie tablicy punktów:
    int k = 0;
    for(k = 0;k<id_gracza;k++){
        printf("ID: %d Punkty: %d\n",k,tablica_punktow[k]);
        fflush(stdout);
    }
    //if(punkty==tablica_punktow[0])
    pthread_mutex_unlock(&punkty_mutex);
    if(punkty==maximum)
        return 1;
    else
        return 0;
   
}

////////////////////////////

//the thread function
void *connection_handler(void *);
 
int main(int argc , char *argv[])
{
    int socket_desc , client_sock , c;
    struct sockaddr_in server , client;
     
    //Create socket
    socket_desc = socket(AF_INET , SOCK_STREAM , 0);
    if (socket_desc == -1)
    {
        printf("Could not create socket");
    }
    puts("Socket created");
     
    //Prepare the sockaddr_in structure
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_port = htons( 8888 );
     
    //Bind
    if( bind(socket_desc,(struct sockaddr *)&server , sizeof(server)) < 0)
    {
        //print the error message
        perror("bind failed. Error");
        return 1;
    }
    puts("bind done");
     
    //Listen
    listen(socket_desc , 3);
     
    //Accept and incoming connection
    //puts("Waiting for incoming connections...");
    //c = sizeof(struct sockaddr_in);
     
     
    //Accept and incoming connection
    puts("Waiting for incoming connections...");
    c = sizeof(struct sockaddr_in);
    pthread_t thread_id;
    
    while( (client_sock = accept(socket_desc, (struct sockaddr *)&client, (socklen_t*)&c)) )
    {
        puts("Connection accepted");
         
        if( pthread_create( &thread_id , NULL ,  connection_handler , (void*) &client_sock) < 0)
        {
            perror("could not create thread");
            return 1;
        }
         
        //Now join the thread , so that we dont terminate before the thread
        //pthread_join( thread_id , NULL);
        puts("Handler assigned");
    }
     
    if (client_sock < 0)
    {
        perror("accept failed");
        return 1;
    }
     
    return 0;
}
 
/*
 * This will handle connection for each client
 * */
void *connection_handler(void *socket_desc)
{
    //Dolicz gracza do puli
    increment_count(); //którym jestem graczem
    int id = get_id_gracza_and_inc();
    id--;

    printf("Jestem graczem: %d\n",id);
    fflush(stdout);
    //Get the socket descriptor
    int sock = *(int*)socket_desc;
    int read_size;
    char *message , client_message[2000];
     
    //Send some messages to the client
    message = "Hejka to ja Twoja obsluga polaczenia! Poczekaj na reszte graczy! \n";
    write(sock , message , strlen(message));
    
    //Jeżeli nie ma minimum 2 graczy to sobie poczekaj na kompana
    while(get_id_gracza()<2){}
    //message = "Now type something and i shall repeat what you type \n";
    message = "Zacznij zgadywac \n";
    write(sock , message , strlen(message));
    /////////////////////////////////////////////////////////////////////
    //Punkty:
    int punkty = 0;
    //Tablica słow:
    const char *slowa[3];
    slowa[0] = "kwiaty";
    slowa[1] = "sandaly";
    slowa[2] = "cokolwiek";

    char hangmanWord[100], tempWord[100];       /**hangmanWord[] array for the original word and tempWord[] array to get the alphabet from user and compare it with original word**/
    char hangmanOutput[100]; 
    char hangmanClientOutput[100];              /**This array will show the remaining blanks and correct inputs**/
    int wrongTry = 6 , matchFound = 0;          /**player will get 5 chance, so we use wrongTry as chance counter**/
                                                /**matchFound to search the alphabet, if the alphabet from user does not exist
                                                in the original word it will remain 0, upon finding the word, matchFound will
                                                be set as 1**/
    int counter = 0 , position = 0, winner=100, length , i;//winner ==100 losowa liczba byleby nie 0
    char alphabetFromUser;

    //Wczytaj slowo z tablicy

    //scanf("%s",hangmanWord);
    int licznik_slow = 0;
    for(licznik_slow = 0;licznik_slow<3;licznik_slow++)
    {
        int licznik = 0;
        printf("Gracz numer: %d Slowo numer: %d\n",id,licznik_slow);
        fflush(stdout);
        printf("%d",licznik_slow);
        int len = strlen(slowa[licznik_slow]);
        for(licznik = 0;licznik<len;licznik++){
            hangmanWord[licznik] = slowa[licznik_slow][licznik];}
    
        length = strlen(hangmanWord);
        printf("%d",length);


        //message = "\n\n     The word has %d alphabets \n\n";
        //write(sock , message , strlen(message));
        //int tmp = htonl(length);
        //write(sock, &tmp, 4);


        //char literka;
        /////////////////////////////////////////////////////////////////////
        for( i = 0; i < length ; i++)
            {
                hangmanOutput[i] = '_';
                hangmanOutput[length] = '\0';

                hangmanClientOutput[i] = '_';
                hangmanClientOutput[length] = '\n';
            }
        //write(sock , hangmanOutput , strlen(hangmanOutput));
        write(sock , hangmanClientOutput , strlen(hangmanClientOutput));

        //Receive a message from client
        while(wrongTry != 0) {
                read_size  = recv(sock , client_message , 2000 , 0);
                matchFound = 0;

                
                //end of string marker
                client_message[read_size] = '\0';
                alphabetFromUser = client_message[0];


                //Sprawdź czy to nie jest ostatni gracz i czy nie należy zakończyć rozgrywki:
                if(get_count() == 1)
                {
                    puts("Zostal jeden gracz");
                    //write(fd[1],&czy_ostatni,1); //Wyślij pozostałym wątkom info, że koniec gry
                    //puts("Blokada wysylania!");
                    //int shut = shutdown(sock,SHUT_RD);
                    dodaj_swoje_punkty(punkty,id);
                    int wynik = czy_wygrana(id,punkty);
                    printf("Id gracza: %d Wynik: %d \n",id,wynik);
                    fflush(stdout);
                    if(wynik == 0)
                    {
                        message = "Przegrana gra! ";
                        write(sock , message , strlen(message));
                        //puts("Nie wygrales");
                    }
                    else if(wynik > 0)
                    {
                        message = "Wygrana gra! \n";
                        write(sock , message , strlen(message));
                        //puts("Wgrales");
                    }
                    close(sock); //Zamknij socket
                    return 0;
                }

                if(alphabetFromUser < 'a' || alphabetFromUser > 'z') /**In case player gives input other than 'a' to 'z' the console will ask again**/
                {
                    //message = "\n\n\t Wrong input TRY AGAIN ";
                    message = "Wrong input TRY AGAIN \n";
                    write(sock , message , strlen(message));
                    matchFound = 2;
                }
                if (matchFound != 2)
                {
                    for(counter=0;counter<length;counter++)    /**for loop to check whether player input alphabet exists or not in the word**/
                    {
                        if(alphabetFromUser==hangmanWord[counter])
                        {
                            matchFound = 1;
                            punkty++; //Zwiększ o jeden punkt za znalezienie litery
                            dodaj_swoje_punkty(punkty,id);
                        }//end of if()
                    }//end of for()
                    if(matchFound == 0)                      /**in case of wrong guess**/
                    {
                        //printf("\n\t :( You have %d tries left ",--wrongTry);
                        //getchar();
                        //showHangman(wrongTry);
                        //getchar();
                        wrongTry--;
                        message = "Marny koniec nadchodzi \n";
                        write(sock , message , strlen(message));
                    }//end of if()
                    else
                    {
                        for(counter = 0; counter < length; counter++)
                        {
                            matchFound = 0;
                            if(alphabetFromUser == hangmanWord[counter])
                            {
                                position = counter ;
                                matchFound = 1;
                            }//end of if
                            if(matchFound == 1)
                            {
                                for(i = 0 ; i < length ; i++)
                                {
                                    if( i == position)
                                    {
                                        hangmanOutput[i] = alphabetFromUser; /**Put the alphabet at right position**/
                                        //
                                        hangmanClientOutput[i] = alphabetFromUser;
                                    }
                                    else if( hangmanOutput[i] >= 'a' && hangmanOutput[i] <= 'z' ) /** If the position already occupied
                                                                                      by same alphabet then no need to
                                                                                      fill again EASY!! and continue */
                                    {
                                        continue;
                                    }
                                    else
                                    {
                                        hangmanOutput[i] = '_';            /** Put a blank at not guessed alphabet position **/
                                        //
                                        hangmanClientOutput[i] = '_';
                                    }
                                }
                                tempWord[position] = alphabetFromUser;      /**put the alphabet in another char array to check with the original word**/
                                tempWord[length] = '\0';                    /**put the NULL character at the end of the temp string**/
                                winner = strcmp(tempWord,hangmanWord); /**upon True comparison it will return 0**/
                                if(winner == 0)                             /**if the player guessed the whole word right then he/she is the WINNER**/
                                {   
                                    //message = "\n\n\t \t";
                                    //write(sock , message , strlen(message));
                                    hangmanWord[strlen(hangmanWord)] = '\n';
                                    write(sock , hangmanWord , strlen(hangmanWord));
                                    //message = "\n\n\t \t YAHOO!!!!! You are the WINNER !!!!!";
                                    message = "Zgadles!\n";
                                    write(sock , message , strlen(message));
                                    //printf("\n\n\t The Word was %s ",hangmanWord);
                                    //printf("\n\n\n\n\t\tEASY HUH???\n\n");
                                    //getchar();

                                    //return 0; //UWAGA TO BYŁO ODKOMENTOWANE
                                }//end of inner if
                            }//end of outer if
                        }//end of for loop
                    }//end of else
                }// end of if(matchFound != 2) condition

                //printf("\n\n\t");
                //for(i = 0 ; i < length ; i++)
                //{
                    //printf(" ");
                    //printf("%c",hangmanOutput[i]);                /**Show the original Word With blanks and right Input alphabet**/
             //UWAGA ZAMIANA NA ZNAK \n NA KOŃCU:   
                    //write(sock , hangmanOutput , strlen(hangmanOutput));
                    write(sock , hangmanClientOutput , strlen(hangmanClientOutput));

                    printf("Gracz: %d, moze zrobic: %d bledow.\n",id,wrongTry);
                    fflush(stdin);  
                    
                    //Wyczyść output dla klienta przed następnym słowem
                    if(winner == 0 )
                    {   
                        if(licznik_slow!=2){
                        message = "Pora na kolejne slowo!\n";
                        write(sock , message , strlen(message));} 
                        //Czyszczenie
                        int x = 0;
                        for(x = 0;x<length;x++)
                        hangmanClientOutput[x] = '\0';
                        //puts("Olaboga");
                        break;
                    }          
                //}
                //puts("1");
                    if(wrongTry == 0)
                    {
                        message = "Straciles szansy, poczekaj na wyniki!\n";
                        write(sock , message , strlen(message));     
                        int shut = shutdown(sock,SHUT_RD);   

                        //Gracz odpadł, odejmij od puli
                        decrement_count();

                        while(get_count()!=1)
                        {
                            //printf("Ile graczy: %lld\n",get_count());
                        }
                         printf("Wylazlem!: %lld %d\n",get_count(),id);
                         fflush(stdin);
                        if(get_count() == 1)
                        {
                            dodaj_swoje_punkty(punkty,id);
                            int wynik = czy_wygrana(id,punkty);
                            if(wynik == 0)
                            {
                                message = "Przegrana gra! \n";
                                write(sock , message , strlen(message));
                                puts("Nie wygrales");
                            }
                            else
                            {
                                message = "Wygrana gra! \n";
                                write(sock , message , strlen(message));
                                puts("Wgrales");
                            }
                            close(sock); //Zamknij socket
                            return 0;
                        }

                    }
                //puts("2");
                /////////////////////////////////////////////////////////////////
                //for( i = 0; i < length ; i++)
                //{
                //    hangmanOutput[i] = '_';
                //    hangmanOutput[length] = '\0';
                //}
                //write(sock , hangmanOutput , strlen(hangmanOutput));
                //literka = client_message[0];


                /////////////////////////////////////////////////////////////////

                //Send the message back to client
                //write(sock , client_message , strlen(client_message));
            
                //clear the message buffer
                memset(client_message, 0, 2000);
            //}
            //puts("3");
            if(read_size == 0)
            {
                puts("Client disconnected");
                fflush(stdout);
            }
            else if(read_size == -1)
            {
                perror("recv failed");
            }
      
        }

        //Wyczyść output dla klienta przed następnym słowem
        int x = 0;
        for(x = 0;x<length;x++)
        hangmanClientOutput[x] = '\0';        
        //Wyczyść słowo - bo pora na nowe
        int g = 0;
        for(g = 0;g<100;g++)
        hangmanWord[g] = '\0';

    } //licznik_slow
    message = "Zgadles wszystko poczekaj na wyniki! \n";
    write(sock , message , strlen(message));

    //Jeżeli zgadłeś wszystkie słowa i czekasz na wyniki to:
    decrement_count();

    while(get_count()!=1)
    {
         //printf("Ile graczy: %lld\n",get_count());
    }
    printf("Wylazlem!: %lld %d\n",get_count(),id);
    if(get_count() == 1)
    {
        dodaj_swoje_punkty(punkty,id);
        int wynik = czy_wygrana(id,punkty);
        if(wynik == 0)
        {
            message = "Przegrana gra! \n";
            write(sock , message , strlen(message));
            puts("Nie wygrales");
        }
        else
        {
            message = "Wygrana gra! \n";
            write(sock , message , strlen(message));
            puts("Wgrales");
        }
            close(sock); //Zamknij socket
            return 0;
    }


        return 0;
} 
