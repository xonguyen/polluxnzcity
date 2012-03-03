/* https://banu.com/blog/2/how-to-use-epoll-a-complete-example-in-c/
 * http://stackoverflow.com/questions/27247/could-you-recommend-some-guides-about-epoll-on-linux
 */

#include <fcntl.h>      // read
#include <stdio.h>      // printf
#include <unistd.h>     //open
#include <string.h>     // memset
#include <errno.h>
#include <stdlib.h>
#include <termios.h>
#include <sys/epoll.h>

#define EPOLL_MAX_CONN 2
#define EPOLL_RUN_TIMEOUT -1
#define BUFF_SIZE 50

class SerialException {
    int code;
    public:
        SerialException(int i) : code(i) {}
        void print_msg() {
            printf("Serial Error #%d: %s\n", code, strerror(code));
        }
        int get_code() {
            return code;
        }
};

class Serial {
    const char* port;

    int epfd;
    int fd;
    struct epoll_event events;

    public:
        Serial(const char* port) : port(port) {
        }

        int begin(int speed) {
            printf("Serial.begin(%d)\n", speed);
            int res;
            struct epoll_event ev;
            struct termios my_termios;
            
            fd = open(port, O_RDWR | O_NOCTTY | O_NONBLOCK);
            if (fd == -1) {
                perror("Unable to open serial port.");
                return -1;
            }

            /* Thanks to Milkymist project to have found Julien Schmitt's work (GTKTerm) 
             * for figuring out the correct parameters to put into that weird struct.
             */
            tcgetattr(fd, &my_termios);
            my_termios.c_cflag = speed;
            my_termios.c_cflag |= CS8;
            my_termios.c_cflag |= CREAD;
            my_termios.c_iflag = IGNPAR | IGNBRK;
            my_termios.c_cflag |= CLOCAL;
            my_termios.c_oflag = 0;
            my_termios.c_lflag = 0;
            my_termios.c_cc[VTIME] = 0;
            my_termios.c_cc[VMIN] = 1;
            tcsetattr(fd, TCSANOW, &my_termios);
            tcflush(fd, TCOFLUSH);
            tcflush(fd, TCIFLUSH);

            epfd = epoll_create(EPOLL_MAX_CONN);
            ev.events = EPOLLIN | EPOLLPRI | EPOLLERR | EPOLLHUP;
            ev.data.fd = fd;

            res = epoll_ctl(epfd, EPOLL_CTL_ADD, fd, &ev);
            if( res < 0 ) {
                printf("Error epoll_ctl: %i\n", errno);
                return -1;
            }
            return 1;
        }

        int poll() {
            //printf("poll()\n");
            
            int n = epoll_wait(epfd, &events, EPOLL_MAX_CONN, EPOLL_RUN_TIMEOUT);

            if(n < 0)
                perror("Epoll failed");
            else if(n==0)
                printf("TIMEOUT\n");
            else {
                recv();
            }
        }

        virtual void recv() {
            try {
                printf("Input: ");
                char c=-1;
                while ((c = this->read()) != -1)
                    printf("%x",c);
                printf("\n");
            } catch (SerialException e) {
                printf("\n");
                e.print_msg();
            }
        }

        virtual char read() {
            int size;
            //char buff[BUFF_SIZE];
            char c;
            //memset(buff, 0, BUFF_SIZE);

            size = ::read(fd, &c, 1);
            if( size > 0 )
                return c;
            else if( size == 0 || errno == 11) {
                //printf("No input\n");
                return -1;
            }
            throw(SerialException(errno));
        }

        virtual ssize_t write(char* data, int len) {
            //printf("Serial.write(%s)\n", data);
            ::write(fd, data, len);
        }

        virtual ssize_t write(uint8_t data) {
            //printf("\nSerial.write(%x)\n", data);
            ::write(fd, &data, sizeof(data));
        }
};
