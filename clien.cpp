#include <stdio.h>      
#include <stdlib.h>     
#include <unistd.h>     
#include <string.h>    
#include <sys/socket.h> 
#include <netinet/in.h> 
#include <netdb.h>
#include <arpa/inet.h>
#include <map>
#include <string>
#include <stack>
#include <iostream>
#include "nlohmann/json.hpp"
#include "requests.h"
#include "buffer.h"
#include "helpers.h"
using std :: string;
using namespace std;

using json = nlohmann::json;

int main(int argc, char *argv[])
{
    
    int ok = 0;
    char *message;
    char *response;
    int sockfd;
    string login = string("");
    string token = string("");

    while(1) {
        
        char *command = (char *)malloc(50 * sizeof(char));
        // fgets(command, 50, stdin);
        cin >> command;

        if(strcmp(command, "register") == 0) {
            string username;
            cout << "username=";
            ok = 1;
            cin >> username;
            string password;
            cout << "password=";
            cin >> password;
            ok = 0;
            for(int i = 0; i < username.length() - 1; i++)
                if(username[i] == ' ')
                ok = 1;
            for(int i = 0; i < password.length() - 1; i++)
                if(password[i] == ' ')
                    ok = 1;
            if(ok == 1) {
                cout << "The username and the password cannot contain white spaces" << endl;
            }else {
            json j = {
                {"username", username},
                {"password", password},
            };
            char* message = compute_post_request("34.254.242.81:8080", "/api/v1/tema/auth/register", "application/json", j.dump(), 1, nullptr, 0);

            int connection_opener =  open_connection("34.254.242.81", 8080, PF_INET, SOCK_STREAM, 0);
            send_to_server(connection_opener, message);
            char* response = receive_from_server(connection_opener);

            char *ptr = strstr(response, "error");
            if (ptr != NULL) {
                cout << "Error! The username " << username << " is taken!" << endl;
            } else {
                cout << "User added successfully!" << endl;
            }


            close_connection(connection_opener);
            }

        } else if(strcmp(command, "login") == 0) {
            if(login.compare("") !=0){
                cout<<"There is a user already logged in. Please logout before trying to login!"<<endl;

            }else{
            string username;
            cout << "username=";
            ok = 1;
            cin >> username;

            string password;
            cout << "password=";
            cin >> password;
            ok = 0;
            json j = {
                {"username", username},
                {"password", password},
            };
            char* message = compute_post_request("34.254.242.81:8080", "/api/v1/tema/auth/login", "application/json", j.dump(), 1, nullptr, 0);

            int connection_opener =  open_connection("34.254.242.81", 8080, PF_INET, SOCK_STREAM, 0);
            send_to_server(connection_opener, message);
            char* response = receive_from_server(connection_opener);

            
            char *ptr = strstr(response, "error");
            if (ptr != NULL) {
                cout << ptr << endl;
            } else {
                string response2 = string(response);
                unsigned cookie_start = response2.find("Set-Cookie");
                unsigned cookie_end = response2.find("Date");
                login = response2.substr(cookie_start + 12, cookie_end - cookie_start - 14);
                login = login + ";";

                
                
                cout << "Login successfully!" << endl;
            }


            close_connection(connection_opener);
            }


        } else if(strcmp(command, "enter_library") == 0) {
            if(login.compare("") == 0){
                cout<<"There is no user logged in\n";
            } else {
                string cookies[1];
                cookies[0] = "Cookie: " + login;

                char* message = compute_get_request("34.254.242.81:8080", "/api/v1/tema/library/access", NULL, cookies, 1);
                int connection_opener =  open_connection("34.254.242.81", 8080, PF_INET, SOCK_STREAM, 0);
                send_to_server(connection_opener, message);
                char* response = receive_from_server(connection_opener);
                char *ptr = strstr(response, "error");
                if (ptr != NULL) {
                    cout << ptr << endl;
                } else {
                    string response2 = string(response);
                    unsigned token_start = response2.find("token");
                    unsigned token_end = response2.find("}");
                    token = response2.substr(token_start + 8, token_end - token_start -9 );
                    token = "Authorization: Bearer " + token;
                    cout << "Library entered successfully" <<endl;

                }
            
            close_connection(connection_opener);
            }
        } else if(strcmp(command, "get_books") == 0) {
            if(login.compare("") == 0){
                cout<<"There is no user logged in\n";
            } else {
                if(token.compare("") == 0) {
                    cout << "Before entering this command you have to enter the library\n";
                } else {
                    string cookies[2];
                cookies[0] = token;
                cookies[1] = "Cookie: " + login;

                char* message = compute_get_request("34.254.242.81:8080", "/api/v1/tema/library/books", NULL, cookies, 2);

                int connection_opener =  open_connection("34.254.242.81", 8080, PF_INET, SOCK_STREAM, 0);
                send_to_server(connection_opener, message);
                char* response = receive_from_server(connection_opener);
                char *ptr = strstr(response, "error");
                if (ptr != NULL) {
                    cout << ptr << endl;
                } else {
                    if (string(response).find("[{") == std::string::npos || string(response).find("}]") == std::string::npos)
                        cout << "This user's library is empty\n";
                    else {
                            string str = string(response);
                            unsigned first = str.find("[{");
                            unsigned last = str.find("}]");
                            string to_print = str.substr(first, last - first + 2);
                            json array_json = json::parse(to_print);
                            for (int i = 0; i < static_cast<int>(array_json.size()); ++i) {
                            cout << "id: " << array_json[i]["id"] << "," << endl;
                            cout << "title: " << array_json[i]["title"] << endl;
                            cout <<endl;
                        }
                    }
                }

                close_connection(connection_opener);
                }
                
            }
        } else if(strcmp(command, "get_book") == 0) {
            if(login.compare("") == 0){
                cout<<"There is no user logged in\n";
            } else {
                if(token.compare("") == 0) {
                    cout << "Before entering this command you have to enter the library\n";
                } else {
                    string bookID;
                    cout << "id=";
                    ok = 1;
                    cin>>bookID;
                    char string_to_put[50] = "/api/v1/tema/library/books/";
                    char integer_string[32];
                    ok = 0;
                    string numbers = "0123456789";
                    int contor = 0;
                    for(int i = 0; i < bookID.length(); i++){
                        size_t ptr = numbers.find(bookID[i]);
                        if(ptr > 10)
                            contor ++;
                    }
                    if(contor > 0){
                        cout << "The id must be a valid NUMBER" <<endl;
                    }
                    else {
                    int book_id = stoi(bookID);
                    sprintf(integer_string, "%d", book_id);
                    strcat(string_to_put, integer_string);

                    string cookies[2];
                    cookies[0] = token;
                    cookies[1] = "Cookie: " + login;
                    char* message = compute_get_request("34.254.242.81:8080", string_to_put, NULL, cookies, 2);

                    int connection_opener =  open_connection("34.254.242.81", 8080, PF_INET, SOCK_STREAM, 0);
                    send_to_server(connection_opener, message);
                    char* response = receive_from_server(connection_opener);
                    char *ptr = strstr(response, "error");
                if (ptr != NULL) {
                    cout << ptr << endl;
                } else {
                    string str =string(response);
                    unsigned first = str.find("{");
                    unsigned last = str.find("}");

                    string to_print = str.substr(first, last - first + 2);

                    json array_json = json::parse(to_print);

                    cout << "id: " << book_id << endl;
                    cout << "title: " << array_json["title"] << endl;
                    cout << "author: " << array_json["author"] << "," << endl;
                    cout << "publisher: " << array_json["publisher"] << ","
                        << endl;
                    cout << "genre: " << array_json["genre"] << "," << endl;
                    cout << "page_count: " << array_json["page_count"] << endl;
                    
                    close_connection(connection_opener);
                }
                    }
                }
            }   
        } else if(strcmp(command, "add_book") == 0) {
            if(login.compare("") == 0){
                cout<<"There is no user logged in\n";
            } else {
                if(token.compare("") == 0) {
                    cout << "Before entering this command you have to enter the library\n";
                } else {
                    string cookies[2];
                    cookies[0] = token;
                    cookies[1] = "Cookie: " + login;
                   
                    string title;
                    cout << "title=";
                    ok = 1;
                    cin >>title;

                    string author;
                    cout << "author=";
                    cin >> author;

                    string genre;
                    cout << "genre=";
                    cin >> genre;
                    
                    string publisher;
                    cout << "publisher=";
                    cin >> publisher;

                    string page_count;
                    string numbers = "0123456789";
                    cout << "page_count=";
                    cin >> page_count;

                    int contor = 0;
                    for(int i = 0; i < page_count.length(); i++){
                        size_t ptr = numbers.find(page_count[i]);
                        if(ptr > 10)
                            contor ++;
                    }
                    if(contor > 0){
                        cout << "The page_count must be a valid NUMBER" <<endl;
                    }
                    else {
                    ok = 0;
                    json j = {
                        {"title", title},
                        {"author", author},
                        {"genre", genre},
                        {"page_count", page_count},
                        {"publisher", publisher},
                    };
                    char* message = compute_post_request("34.254.242.81:8080", "/api/v1/tema/library/books", "application/json", j.dump(), 1, cookies, 2);

                    int connection_opener =  open_connection("34.254.242.81", 8080, PF_INET, SOCK_STREAM, 0);
                    send_to_server(connection_opener, message);
                    char* response = receive_from_server(connection_opener);
                    char *ptr = strstr(response, "error");

                    if (ptr != NULL) {
                        cout << response  << endl;
                    } else {
                        cout << "Book successfully added!" << endl;
                    }
                    close_connection(connection_opener);
                    }
                }
            }

            
        } else if(strcmp(command, "delete_book") == 0) {
            if(login.compare("") == 0){
                cout<<"There is no user logged in\n";
            } else {
                if(token.compare("") == 0) {
                    cout << "Before entering this command you have to enter the library\n";
                } else {
                    string bookID;
                    ok = 1;

                    cout << "id=";
                    cin>>bookID;
                    int contor = 0;
                    string numbers = "0123456789";
                    for(int i = 0; i < bookID.length(); i++){
                        size_t ptr = numbers.find(bookID[i]);
                        if(ptr > 10)
                            contor ++;
                    }
                    if(contor > 0){
                        cout << "The id must be a valid NUMBER" <<endl;
                    }
                    else {
                    ok = 0;
                    char string_to_put[50] = "/api/v1/tema/library/books/";
                    char integer_string[32];
                    int bookID_no = stoi(bookID);
                    sprintf(integer_string, "%d", bookID_no);
                    strcat(string_to_put, integer_string);

                    string cookies[2];
                    cookies[0] = token;
                    cookies[1] = "Cookie: " + login;
                    char* message = compute_delete_request("34.254.242.81:8080", string_to_put, NULL, cookies, 2);


                    int connection_opener =  open_connection("34.254.242.81", 8080, PF_INET, SOCK_STREAM, 0);
                    send_to_server(connection_opener, message);
                    char* response = receive_from_server(connection_opener);
                    char *ptr = strstr(response, "error");
                    if (ptr != NULL) {
                        cout << ptr  << endl;
                    } else {
                        cout << "Book successfully deleted!" << endl;
                    }

                    close_connection(connection_opener);
                    }
                }
            }   
        } else if(strcmp(command, "logout") == 0) {
             if(login.compare("") == 0){
                cout<<"There is no user logged in\n";
            } else {
                string cookies[1];
                cookies[0] = "Cookie: " + login;
                char* message = compute_get_request("34.254.242.81:8080", "/api/v1/tema/auth/logout", NULL, cookies, 1);
            int connection_opener =  open_connection("34.254.242.81", 8080, PF_INET, SOCK_STREAM, 0);
            send_to_server(connection_opener, message);
            response = receive_from_server(connection_opener);
            char *ptr = strstr(response, "error");
            if (ptr != NULL) {
                cout << ptr << endl;
            } else {
                cout << "Logout successful" << endl;
            }
            login = "";
            token = "";
            close_connection(connection_opener);
            }   
        } else if(strcmp(command, "exit") == 0) {
            break;
        }
    }

    return 0;
}
