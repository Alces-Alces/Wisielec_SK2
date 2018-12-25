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
    puts("Waiting for incoming connections...");
    c = sizeof(struct sockaddr_in);
     
     
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
    //Get the socket descriptor
    int sock = *(int*)socket_desc;
    int read_size;
    char *message , client_message[2000];
     
    //Send some messages to the client
    message = "Greetings! I am your connection handler\n";
    write(sock , message , strlen(message));
     
    //message = "Now type something and i shall repeat what you type \n";
    message = "Now type in server word to guess and I will show you length of the word \n";
    write(sock , message , strlen(message));
    /////////////////////////////////////////////////////////////////////
    char hangmanWord[100], tempWord[100];       /**hangmanWord[] array for the original word and tempWord[] array to get the alphabet from user and compare it with original word**/
    char hangmanOutput[100]; 
    char hangmanClientOutput[100];              /**This array will show the remaining blanks and correct inputs**/
    int wrongTry = 6 , matchFound = 0;          /**player will get 5 chance, so we use wrongTry as chance counter**/
                                                /**matchFound to search the alphabet, if the alphabet from user does not exist
                                                in the original word it will remain 0, upon finding the word, matchFound will
                                                be set as 1**/
    int counter = 0 , position = 0, winner, length , i;
    char alphabetFromUser;

    scanf("%s",hangmanWord);
    length = strlen(hangmanWord);
    //printf("%d",length);


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
    //while(wrongTry != 0 && (read_size = recv(sock , client_message , 2000 , 0)) > 0){
    while(wrongTry != 0) {
        read_size  = recv(sock , client_message , 2000 , 0);
        matchFound = 0;

        //while( (read_size = recv(sock , client_message , 2000 , 0)) > 0 )
        //{
            //end of string marker
            client_message[read_size] = '\0';
            alphabetFromUser = client_message[0];

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
                    }//end of if()
                }//end of for()
                if(matchFound == 0)                      /**in case of wrong guess**/
                {
                    //printf("\n\t :( You have %d tries left ",--wrongTry);
                    //getchar();
                    //showHangman(wrongTry);
                    //getchar();
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
                                message = "YAHOO!!!!! You are the WINNER !!!!!\n";
                                write(sock , message , strlen(message));
                                //printf("\n\n\t The Word was %s ",hangmanWord);
                                //printf("\n\n\n\n\t\tEASY HUH???\n\n");
                                //getchar();
                                return 0;
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
            //}
        

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
        return 0;
} 
