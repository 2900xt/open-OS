#ifndef IO_HPP
#include <io.hpp>
#endif

#ifndef STRING_HPP
#define STRING_HPP


namespace GSL{

    class String{
        char* data;
        int length;

        public:

            String(const char* str){
                length = strlen(str);
                data = (char*)malloc(length);
                memcpy(data,str,length);
            }

            String(){
               length = 0;
               data = 0; 
            }

            char* c_str(){
                return data;
            }

            int size(){
                return length;
            }

            void operator=(const char* str){
                length = strlen(str);
                data = (char*)malloc(length);
                memcpy(data,str,length);
            }

            char operator[](qword index){
                if(index > length){
                    return 0;
                }
                return data[index];
            }

            void operator<<(const char* str){
                data = (char*)realloc(data,strlen(str));
                strcat(data,(char*)str);
            }
            void operator<<(String& str){
                data = (char*)realloc(data,strlen(str.c_str()));
                strcat(data,(char*)str.c_str());
            }
            void operator>>(String& str){
                str = data;
                return;
            }

            void operator=(String str){
                memcpy(data,str.c_str(),str.size());
            }

            ~String(){
                free(data);
            }
    };


}

#endif