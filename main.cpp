#include <SDL/SDL.h>
#include <SDL/SDL_image.h>
#include <SDL/SDL_ttf.h>
#include <math.h>
#include <stdlib.h>

#define WIDTH 480
#define HEIGHT 272

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <netdb.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#include "rabbit.h"

#include <iostream>
#include <boost/filesystem.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/algorithm/string_regex.hpp>
#include <boost/format.hpp>

#include <boost/date_time/posix_time/posix_time.hpp>
#include <iostream>
#include <sstream>

using namespace std;
using namespace boost::filesystem;

std::string FormatTime(boost::posix_time::ptime now)
{
  using namespace boost::posix_time;
  static std::locale loc(std::cout.getloc(),
                         new time_facet("%H:%M"));

  std::basic_stringstream<char> wss;
  wss.imbue(loc);
  wss << now;
  return wss.str();
}

TTF_Font *open_font(const char *file, int size)
{
  TTF_Font *font = TTF_OpenFont(file, size);
  if (!font) {
    printf("Font error %s!\n", file);
    exit(-1);
  }
  return font;
}

void test_sdl() {
  int gogogo = 1;
  int n = 0;
  SDL_Event event;
  SDL_Surface *screen, *png_image;
  Uint32 currentTime;

  SDL_Init(SDL_INIT_VIDEO);
  TTF_Init();
  Uint32 lastTime = SDL_GetTicks();
  SDL_WM_SetCaption("Hello World! :D", NULL);
  screen = SDL_SetVideoMode(WIDTH, HEIGHT, 32, SDL_SWSURFACE | SDL_ANYFORMAT | SDL_DOUBLEBUF);

  png_image = IMG_Load("./background.png");

  TTF_Font *chineese_font = open_font("babel.ttf", 20);
  TTF_Font *eu_font = open_font("freesansbold.ttf", 20);


  SDL_Color col_white;
  col_white.r = 255;
  col_white.g = 255;
  col_white.b = 255;

  char char_array[] = u8"你好吗";

  SDL_Surface *chineese_text = TTF_RenderUTF8_Blended(chineese_font, char_array, col_white);
  TTF_CloseFont(chineese_font);

  char char_array2[] = u8"Hällö";

  SDL_Surface *eu_text = TTF_RenderUTF8_Blended(eu_font, char_array2, col_white);
  TTF_CloseFont(eu_font);

  while (gogogo) {
    if (SDL_PollEvent(&event) == 0) {
      SDL_Rect rect;
      rect.x = 0;
      rect.y = 0;
      rect.w = 0;
      rect.h = 0;
      currentTime = SDL_GetTicks();

      SDL_BlitSurface(png_image, NULL, screen, &rect);

      SDL_Surface *text = (currentTime / 1000) % 2 == 0 ? chineese_text : eu_text;
      SDL_BlitSurface(text, NULL, screen, &rect);

      rect.x = (int) ((1. + .5 * sin(0.03 * n)) * (WIDTH / 3));
      rect.y = (int) ((1. + .5 * cos(0.03 * n)) * (HEIGHT / 3));
      rect.w = WIDTH * 0.25;
      rect.h = HEIGHT * 0.25;

      SDL_FillRect(screen, &rect, SDL_MapRGB(screen->format, 255, 255, 0));
      SDL_Flip(screen);

      if (currentTime > lastTime + 20) {
        lastTime = currentTime;
        n++;
      }
    }
    else {
      if (event.type == SDL_QUIT)
        gogogo = 0;
    }
  }
  SDL_Quit();

  cerr << "Test SDL OK" << endl;
}

int sendall(int s, char *buf, int *len) {
  int total = 0;
  // how many bytes we've sent
  int bytesleft = *len; // how many we have left to send
  int n = 0;
  while (total < *len) {
    n = send(s, buf + total, bytesleft, 0);
    if (n == -1) { break; }
    total += n;
    bytesleft -= n;
  }
  *len = total; // return number actually sent here
  return n == -1 ? -1 : 0; // return -1 on failure, 0 on success
}

int receiveall(int s, char *buf, int *len) {
  int total = 0;
  // how many bytes we've sentAF_UNSPEC
  int bytesleft = *len; // how many we have left to recv
  int n = 0;
  while (total < *len) {
    n = recv(s, buf + total, bytesleft, 0);
    if (n == -1) { break; }
    total += n;
    bytesleft -= n;
  }
  *len = total; // return number actually sent here
  return n == -1 ? -1 : 0; // return -1 on failure, 0 on success
}



// get sockaddr, IPv4 or IPv6:
void *get_in_addr(struct sockaddr *sa)
{
  if (sa->sa_family == AF_INET) {
    return &(((struct sockaddr_in*)sa)->sin_addr);
  }

  return &(((struct sockaddr_in6*)sa)->sin6_addr);
}

uint32_t char4_to_uint32_t(char * chars) {
  unsigned num = 0;
  for (int i = 0; i != 4; ++i) {
    num |= (uint8_t)chars[i] << i * 8;
  }

  return num;
}

int test_sockets() {
  int sockfd;
  struct addrinfo hints, *servinfo, *p;
  int rv;
  char s[INET6_ADDRSTRLEN];

  memset(&hints, 0, sizeof hints);
  hints.ai_family = AF_UNSPEC;
  hints.ai_socktype = SOCK_STREAM;

  if ((rv = getaddrinfo("0.0.0.0", "12000", &hints, &servinfo)) != 0) {
    fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
    return 1;
  }

  // loop through all the results and connect to the first we can
  for(p = servinfo; p != NULL; p = p->ai_next) {
    if ((sockfd = socket(p->ai_family, p->ai_socktype,
                         p->ai_protocol)) == -1) {
      perror("client: socket");
      continue;
    }

    if (connect(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
      perror("client: connect");
      close(sockfd);
      continue;
    }

    break;
  }

  if (p == NULL) {
    fprintf(stderr, "client: failed to connect\n");
    return 2;
  }

  inet_ntop(p->ai_family, get_in_addr((struct sockaddr *)p->ai_addr),
            s, sizeof s);
  printf("client: connecting to %s\n", s);

  freeaddrinfo(servinfo); // all done with this structure

  struct RequestFromTcpClient req_from_client;
  req_from_client.sof = char4_to_uint32_t("SOF!");
  req_from_client.mode = '1';
  req_from_client.frequency = '0';

  int sz = sizeof(struct RequestFromTcpClient);
  if (sendall(sockfd, (char*)&req_from_client, &sz)) {
    fprintf(stderr, "sendall: failed\n");
    return -1;
  }

  if (sz != sizeof(struct RequestFromTcpClient)) {
    fprintf(stderr, "sendall: incomplete send: %d bytes\n", sz);
    return -1;
  }

  int cnt = 5;
//  sleep(1);

  while(cnt > 0) {
    sz = sizeof(struct ReplyFromServer);
    struct ReplyFromServer reply_from_server;
    if (receiveall(sockfd, (char*)&reply_from_server, &sz)) {
      fprintf(stderr, "receiveall: failed\n");
      return -1;
    }

    if (sz != sizeof(struct ReplyFromServer)) {
      fprintf(stderr, "receiveall: incomplete send: %d bytes\n", sz);
      return -1;
    }

    fprintf(stdout, "Depth #%d: %d \n", cnt, reply_from_server.sonar_status.depth);
    fflush(stdout);
    cnt--;
  }


  close(sockfd);
  fprintf(stderr, "Test sockets OK\n");

  return 0;
}

void test_boost() {
  std::string great = "Hello"s + " World";
  std::cout << great << std::endl;

  for (auto s : {"on" , "off"}) {
    std::stringstream str;
    str << "camera_stream_page__infrared_";
    str << s;
    std::cout << str.str() << std::endl;
  }

  using namespace boost::posix_time;
  ptime now = second_clock::universal_time();

  std::string ws(FormatTime(now));
  std::cout << ws << std::endl;
  const std::string target_path(".");
  const boost::regex my_filter(".*\\.ttf");

  std::vector<std::string> all_matching_files;

  boost::filesystem::directory_iterator end_itr; // Default ctor yields past-the-end
  for (boost::filesystem::directory_iterator i(target_path); i != end_itr; ++i) {
    // Skip if not a file
    if (!boost::filesystem::is_regular_file(i->status())) continue;

    boost::smatch what;

    if (!boost::regex_match(i->path().filename().string(), what, my_filter)) continue;

    // File matches, store it
    std::cout << i->path().filename().string() << std::endl;
  }

  cerr << "Test boost OK" << endl;
}

void test_boost2() {
  std::string str("<<>>hello world<<>>");

  auto sss = (boost::format("%s.%s") % str % "game_ui_list").str();
  std::cout << "Hw:" << sss << std::endl;

  boost::trim_right(str);
  std::cout << "Hw:" << str << std::endl;

  std::cout << "Hw2:" << std::string(boost::trim_copy(str)) << std::endl;

  string line("test1,test2,test3");
  vector<string> strs;
  boost::split(strs, line, boost::is_any_of(","));

  cout << "* size of the vector: " << strs.size() << endl;
  for (size_t i = 0; i < strs.size(); i++) {
    cout << strs[i] << endl;
  }

  std::vector<std::string> result;
  boost::algorithm::split_regex( result, "one two   three, four", boost::regex( "\\s+" ) ) ;

  for (auto v : result) {
    cout << '"' << v << '"' << endl;
  }

  std::string s1("       ");
  s1.clear();
  cout << "s1:" << '"' << s1 << '"' << endl;

  cerr << "Test boost 2 OK" << endl;
}

int main(int argc, char** argv)
{
  test_sockets();
  test_boost();
  test_boost2();

  test_sdl();
  return 0;
}