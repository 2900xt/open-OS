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

            ~String(){
                free(data);
            }
    };


}

#endif