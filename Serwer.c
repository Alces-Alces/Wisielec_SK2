/*
    Serwer wisielca w C
    KOmpilacja
    gcc server.c -lpthread -w -o server
*/
 
#include<stdio.h>
#include<string.h>    //strlen
#include<stdlib.h>    //strlen
#include<sys/socket.h>
#include<arpa/inet.h> //inet_addr
#include<unistd.h>    //write
#include<pthread.h> //do watkow -lpthread
////File desc for pipe
//int fd[2];
////////////////////////////
pthread_mutex_t koniec_gry_mutex;
pthread_mutex_t count_mutex;
pthread_mutex_t punkty_mutex;
pthread_mutex_t id_mutex;
long long count = 0;
int tablica_punktow[100];
int id_gracza = 0;
int koniec_gry = 0;

void
increment_koniec_gry()
{
	pthread_mutex_lock(&koniec_gry_mutex);
    koniec_gry = koniec_gry + 1;
    pthread_mutex_unlock(&koniec_gry_mutex);
}
int
get_koniec_gry_value()
{
	int wynik;
	pthread_mutex_lock(&koniec_gry_mutex);
    wynik = koniec_gry;
    pthread_mutex_unlock(&koniec_gry_mutex);
    return wynik;
}


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

//Funkcja w watku
void *connection_handler(void *);
 
int main(int argc , char *argv[])
{
    int socket_desc , client_sock , c;
    struct sockaddr_in server , client;
     
    //Utworz socket
    socket_desc = socket(AF_INET , SOCK_STREAM , 0);
    if (socket_desc == -1)
    {
        printf("Could not create socket");
    }
    puts("Socket created");
     
    //Przygotowanie struktury sockaddr_inS
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_port = htons( 8080 );
     
    //Bind
    if( bind(socket_desc,(struct sockaddr *)&server , sizeof(server)) < 0)
    {
        //wyprintuj error
        perror("Error bind failed.");
        return 1;
    }
    puts("bind done");
     
    //Nasluchuj
    listen(socket_desc , 3);
     
    //Zaakceptuj przychodzce polaczenie
    puts("Waiting for incoming connections...");
    c = sizeof(struct sockaddr_in);
    pthread_t thread_id;
    
    //int zaakceptowane = 0;
    while( (client_sock = accept(socket_desc, (struct sockaddr *)&client, (socklen_t*)&c)) )
    {//zaakceptowane++;
        puts("Connection accepted");
         
        if( pthread_create( &thread_id , NULL ,  connection_handler , (void*) &client_sock) < 0)
        {
            perror("could not create thread");
            return 1;
        }
         
        //if(zaakceptowane == 4){while(get_koniec_gry_value()<1){}}
        //Oczekiwanie na skonczenie watkow
        //pthread_join( thread_id , NULL);
        puts("Handler assigned");

        printf("Koniec gry? %d\n",get_koniec_gry_value());
        fflush(stdout);
        if(get_koniec_gry_value()>0)
        	{puts("GRA ZAKONCZONA");
        		return 0;}
        	
    }
     
    if (client_sock < 0)
    {
        perror("accept failed");
        return 1;
    }
     
    return 0;
}
 
/*
 * Ta funkcja zajmie się polaczeniem z kazdym klientem
 * */
void *connection_handler(void *socket_desc)
{
    //Dolicz gracza do puli
    increment_count(); //którym jestem graczem
    int id = get_id_gracza_and_inc();
    id--;

    printf("Jestem graczem: %d\n",id);
    fflush(stdout);
    //Dostan socket descripto
    int sock = *(int*)socket_desc;
    int read_size;
    char *message , client_message[2000];
     
    //Pierwszy gracz musi poczekac na reszte
    if(id == 0){
    message = "Hejka to ja Twoja obsluga polaczenia! Poczekaj na reszte graczy! \nn";
    write(sock , message , strlen(message));}
    
    //Jeżeli nie ma minimum 2 graczy to sobie poczekaj na kompana
    while(get_id_gracza()<2){}
    //message = "Now type something and i shall repeat what you type \n";
    message = "Masz 6 szans. Zacznij zgadywac -";
    write(sock , message , strlen(message));
    /////////////////////////////////////////////////////////////////////
    //Punkty:
    int punkty = 0;
    //Tablica słow:
    const char *slowa[3];
    slowa[0] = "kwiaty";
    slowa[1] = "sandaly";
    slowa[2] = "cokolwiek";

    char hangmanWord[100], tempWord[100];       /*hangmanWord[] na pierwotne slowo i tempWord[] na literke uzytkownika do porownania z oryginalnym slowem*/
    char hangmanOutput[100]; 
    char hangmanClientOutput[100];              /*w tej tablicy bedzie pokazany wynik w postaci pustych pol i poprawnych literek*/
    int wrongTry = 6 , matchFound = 0;          /**gracz ma 6 szans*/
                                                /**matchFound - ma na celu sprawdzic czy literka od uzytkownika jest w zgadyanym slowie, jezeli nie to pozostanie 0, jezeli tak to 1*/
    int counter = 0 , position = 0, winner=100, length , i;//winner ==100 losowa liczba byleby nie 0
    char alphabetFromUser;

    //Wczytaj slowo z tablicy

    //scanf("%s",hangmanWord);
    int licznik_slow = 0;
    for(licznik_slow = 0;licznik_slow<3;licznik_slow++)
    {winner = 100; //losowa liczba byleby nie 0
        int licznik = 0;
        printf("Gracz numer: %d Slowo numer: %d\n",id,licznik_slow);
        fflush(stdout);
        printf("%d",licznik_slow);
        int len = strlen(slowa[licznik_slow]);
        for(licznik = 0;licznik<len;licznik++){
            hangmanWord[licznik] = slowa[licznik_slow][licznik];}
    
        length = strlen(hangmanWord);
        printf("%d",length);


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

        //Odbierz wiadomosc od klienta
        while(wrongTry != 0) {
                read_size  = recv(sock , client_message , 2000 , 0);
                matchFound = 0;

                
                //wiadomosc od klienta
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
                        message = "Przegrana gra! \n";
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
                    increment_koniec_gry();
                    return 0;
                }

                if(alphabetFromUser < 'a' || alphabetFromUser > 'z') /*Jezeli input inny niz od 'a' do 'z' to pozwol wpisac ponownie*/
                {
                    message = "Podales nieprawidlowy znak, sproboj ponownie -";
                    write(sock , message , strlen(message));
                    matchFound = 2;
                }
                if (matchFound != 2)
                {
                    for(counter=0;counter<length;counter++)    /*petla sprawdzajaca czy podana literka istnieje w slowie*/
                    {
                        if(alphabetFromUser==hangmanWord[counter])
                        {
                            matchFound = 1;
                            punkty++; //Zwiększ o jeden punkt za znalezienie litery
                            dodaj_swoje_punkty(punkty,id);
                        }
                    }
                    if(matchFound == 0)                      /*jezeli zla literka zostala podana*/
                    {
                        
                        wrongTry--;
                        //if(wrongTry == 0){//Jeżeli już nie ma żyć to wyślij to z \n jako ostatnią wiadomość tej pętli, jeśli życia jeszcze ma to z -
                        message = "Marny koniec nadchodzi, tracisz jedna szanse -";
                    	//}
                        //else{message = "Marny koniec nadchodzi -";}
                        write(sock , message , strlen(message));
                    }
                    else
                    {
                        for(counter = 0; counter < length; counter++)
                        {
                            matchFound = 0;
                            if(alphabetFromUser == hangmanWord[counter])
                            {
                                position = counter ;
                                matchFound = 1;
                            }
                            if(matchFound == 1)
                            {
                                for(i = 0 ; i < length ; i++)
                                {
                                    if( i == position)
                                    {
                                        hangmanOutput[i] = alphabetFromUser; /*wloz literke na wlasciwe miejsce*/
                                        //
                                        hangmanClientOutput[i] = alphabetFromUser;
                                    }
                                    else if( hangmanOutput[i] >= 'a' && hangmanOutput[i] <= 'z' ) /*jezeli jest juz tu taka sama literka to nie ma potrzeby jej podmieniac*/
                                                                                      
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
                                tempWord[position] = alphabetFromUser;      /*literka trafia do innej tablicy celem porowniania z oryginalnym slowem */
                                tempWord[length] = '\0';                    /*znak NULL na koncu tymczasowego stringa*/
                                winner = strcmp(tempWord,hangmanWord); 		/*jezeli prawda zwroci 0*/
                                if(winner == 0)                             /*jeżeli cale slowo zostalo zgadniete to gracz przechodzi do kolejnego slowa i otrzymuje info ze zgadl*/
                                {   
                                    
                                    hangmanWord[strlen(hangmanWord)] = '-';
                                    write(sock , hangmanWord , strlen(hangmanWord));
                                    
                                    message = "Zgadles!-";
                                    write(sock , message , strlen(message));
                                }
                            }
                        }
                    }
                }

                
                	printf("Id gracza: %d WrongTry: %d \n",id,wrongTry);
                    fflush(stdout);  
                	if(winner != 0 && wrongTry != 0){
                		printf("Id gracza: %d WrongTry: %d Wyswietlam slowo!\n",id,wrongTry);
                    	fflush(stdout);  
                    write(sock , hangmanClientOutput , strlen(hangmanClientOutput));}

                    printf("Gracz: %d, moze zrobic: %d bledow.\n",id,wrongTry);
                    fflush(stdout);  
                    
                    //Wyczyść output dla klienta przed następnym słowem
                    if(winner == 0 )
                    {   
                        if(licznik_slow!=2){
                        message = "Pora na kolejne slowo!-";
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
                        sleep(5);   

                        //Gracz odpadł, odejmij od puli
                        decrement_count();

                        while(get_count()!=1)
                        {
                            //printf("Ile graczy: %lld\n",get_count());
                        }
                         printf("Wylazlem!: %lld %d\n",get_count(),id);
                         fflush(stdout);
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
                            increment_koniec_gry();
                            return 0;
                        }

                    }
            
                //Wyczysc bufor wiadomosci
                memset(client_message, 0, 2000);
      
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
    sleep(5);
    //Jeżeli zgadłeś wszystkie słowa i czekasz na wyniki to:
    decrement_count();

    while(get_count()!=1)
    {
         //printf("Ile graczy: %lld\n",get_count());
    }
    printf("Wylazlem!: %lld %d\n",get_count(),id);
    fflush(stdout);
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
            increment_koniec_gry();
            //kill(getppid(),9);
            return 0;
    }


        return 0;
} 
