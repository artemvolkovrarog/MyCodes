#include "common.h"
#include "Image.h"
#include "Player.h"
#include <fstream>
#define GLFW_DLL
#include <GLFW/glfw3.h>
#include <iomanip>
constexpr GLsizei WINDOW_WIDTH = 1024 + 96, WINDOW_HEIGHT = 1024;
//GLOBAL VARIABLES
std::ifstream lvl_map;
std::ifstream global_map;
int *pos_massive = new int[32*32];

  Image EXIT("./resources/EXIT.png");
  Image pl_image("./resources/MY_SHREK.png");
  Image tres("./resources/TREASURE.png");
  Image tres_anim_1("./resources/TREASURE_ANIM_1.png");
  Image tres_taken("./resources/TREASURE_TAKEN.png");
  Image tres_up("./resources/TREASURE_UP.png");
  Image LOST("./resources/LOST.png");
  Image FINISH("./resources/FINISH.png");
  Image HP_BAR("./resources/HP_BAR.png");
  Image HP("./resources/HP.png");
  int alpha_fade = -1, x_fade, y_fade;
  Pixel *cpy = new Pixel[32*32];
  char finish = 'n';

  char *G_map = new char[25];
  int map_i = 0, map_j = 0, map_i_old = map_i, map_j_old = map_j, prev_move = 0;
  char room ;

void Draw_pic(Point coord, Image &pic, Image &screen, bool treasure = false, int alpha = -1);
void Draw_Area(Image &screen, char var, Player& player, Image &EXIT, Image &pl, int map_i, int map_j, int &prev_move, bool first = true);

struct InputState
{
  bool keys[1024]{}; //массив состояний кнопок - нажата/не нажата
  GLfloat lastX = 400, lastY = 300; //исходное положение мыши
  bool firstMouse = true;
  bool captureMouse         = true;  // Мышка захвачена нашим приложением или нет?
  bool capturedMouseJustNow = false;
} static Input;


GLfloat deltaTime = 0.0f;
GLfloat lastFrame = 0.0f;

void OnKeyboardPressed(GLFWwindow* window, int key, int scancode, int action, int mode)
{
	switch (key)
	{
	case GLFW_KEY_ESCAPE:
		if (action == GLFW_PRESS)
			glfwSetWindowShouldClose(window, GL_TRUE);
		break;
  case GLFW_KEY_1:
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
    break;
  case GLFW_KEY_2:
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    break;
	default:
		if (action == GLFW_PRESS)
      Input.keys[key] = true;
		else if (action == GLFW_RELEASE)
      Input.keys[key] = false;
	}

}
void Draw_HP_BAR(Image &screen, int HPs){
  Point coords{.x = WINDOW_WIDTH - 96, .y = WINDOW_HEIGHT - 64 };
  Draw_pic(coords, HP_BAR, screen);
  for (int i = 0; i < HPs; i++){
    Draw_pic(coords, HP, screen, true);
    coords.x += 32;
  }
}

void Draw_fade(Image& pic, Image &screen, int pos_i, int pos_j, GLFWwindow *window){
  Point draw_coords{.x = pos_i *32, .y = pos_j*32};
  
  int i =0, j = 0;
  if (alpha_fade == 255){
  for (i = 0; i<tileSize; i++){
    for ( j =0; j < tileSize; j++){
      cpy[j + i * 32] = screen.GetPixel(draw_coords.x + j, draw_coords.y + i);
    }
  }
}
    if (alpha_fade >0){
      Draw_pic(draw_coords, pic, screen, false, alpha_fade);
      alpha_fade -=5;
    }

if (alpha_fade == 0){
  alpha_fade = -1;
  for (i = 0; i< tileSize; i++){
    for ( j = 0; j < tileSize; j++){
      screen.PutPixel(draw_coords.x + j,draw_coords.y +  i, cpy[j + i *32]);
    }
  }
}
}


void ProcessTreasure(Player& player, Image& screen, GLFWwindow *window){
  int x_1 = player.Coords().x, x_2 = x_1, x_3 = x_1 + tileSize-1, x_4 = x_3;
    int y_1 = player.Coords().y, y_2 = y_1 + tileSize-1, y_3 = y_2, y_4 = y_1;
    int i_1 = x_1/32, i_2 = x_2/32, i_3 = x_3/32, i_4 = x_4/32;
    int j_1 = y_1/32, j_2 = y_2/32, j_3 = y_3/32, j_4 = y_4/32;
    Point draw_coords {.x = 0, .y =0};

    if (pos_massive[i_1 + j_1 *32] == 6){
      if (player.HPs <3) player.HPs += 1;
      Draw_HP_BAR(screen, player.HPs);
      pos_massive[i_1 + j_1 *32] = 7;
      x_fade = i_1 - 1;
      y_fade = j_1;
      alpha_fade = 255;
      draw_coords.x = i_1 * 32;
      draw_coords.y = j_1*32;
      Draw_pic(draw_coords, player.Floor(), screen);
      Draw_pic(draw_coords, player.Treasure_taken(), screen, true);
    }
    else if(pos_massive[i_2 + j_2 *32] == 6){
      if (player.HPs <3) player.HPs += 1;
      Draw_HP_BAR(screen, player.HPs);
      pos_massive[i_2 + j_2 *32] = 7;
      x_fade = i_2;
      y_fade = j_2 + 1;
      alpha_fade = 255;
      draw_coords.x = i_2 * 32;
      draw_coords.y = j_2*32;
      Draw_pic(draw_coords, player.Floor(), screen);
      Draw_pic(draw_coords, player.Treasure_taken(), screen, true);
    }
    else if(pos_massive[i_3 + j_3 *32] == 6){
      if (player.HPs <3) player.HPs += 1;
      Draw_HP_BAR(screen, player.HPs);
      pos_massive[i_3 + j_3 *32] = 7;
      x_fade = i_3;
      y_fade = j_3+1;
      alpha_fade = 255;
      draw_coords.x = i_3 * 32;
      draw_coords.y = j_3*32;
      Draw_pic(draw_coords, player.Floor(), screen);
      Draw_pic(draw_coords, player.Treasure_taken(), screen, true);
    }
    else if(pos_massive[i_4 + j_4 *32] == 6){
      if (player.HPs <3) player.HPs += 1;
      Draw_HP_BAR(screen, player.HPs);
      pos_massive[i_4 + j_4 *32] = 7;
      x_fade = i_4 + 1;
      y_fade = j_4;
      alpha_fade = 255;
      draw_coords.x = i_4 * 32;
      draw_coords.y = j_4*32;
      Draw_pic(draw_coords, player.Floor(), screen);
      Draw_pic(draw_coords, player.Treasure_taken(), screen, true);
    }
}

 char processPlayerMovement(Player &player, int &map_i, int &map_j, char *map, int &prev_move, Image& screen, GLFWwindow* window)
{
  Point res_move = {.x = 0, .y = 0};
  if ((finish == 'n') || (finish == 'r')){
  if (Input.keys[GLFW_KEY_W])
finish = player.ProcessInput(MovementDir::UP, int(WINDOW_HEIGHT), pos_massive, map_i, map_j, prev_move);
  else if (Input.keys[GLFW_KEY_S])
finish = player.ProcessInput(MovementDir::DOWN, 0, pos_massive, map_i, map_j, prev_move);
 if ((Input.keys[GLFW_KEY_A]) && (finish != 'r'))
finish = player.ProcessInput(MovementDir::LEFT, 0, pos_massive, map_i, map_j, prev_move);
  else if (Input.keys[GLFW_KEY_D] && (finish != 'r'))
finish = player.ProcessInput(MovementDir::RIGHT, int(WINDOW_WIDTH), pos_massive, map_i, map_j, prev_move);
else if (Input.keys[GLFW_KEY_E])
ProcessTreasure(player, screen, window);
}
if (finish == 'l'){
  Draw_HP_BAR(screen, player.HPs);
  Point coords{.x = WINDOW_WIDTH/4, .y = WINDOW_HEIGHT/4};
  Draw_pic(coords, LOST, screen);

}
else if (finish == 'f'){
  Point coords{.x = WINDOW_WIDTH/4, .y = WINDOW_HEIGHT/4};
  Draw_pic(coords, FINISH, screen);
}
else if (finish == 'r') {
  Draw_HP_BAR(screen, player.HPs);
  /*player.Coords() = player.spawn_coords;
  player.Old_coords() = player.spawn_coords;
  //prev_move = 0;*/

  Draw_Area(screen, room, player, EXIT, pl_image, map_i, map_j, prev_move, false);
  

}
return map[map_i + map_j*5];
}

void OnMouseButtonClicked(GLFWwindow* window, int button, int action, int mods)
{
  if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_RELEASE)
    Input.captureMouse = !Input.captureMouse;

  if (Input.captureMouse)
  {
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    Input.capturedMouseJustNow = true;
  }
  else
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);

}

void OnMouseMove(GLFWwindow* window, double xpos, double ypos)
{
  if (Input.firstMouse)
  {
    Input.lastX = float(xpos);
    Input.lastY = float(ypos);
    Input.firstMouse = false;
  }

  GLfloat xoffset = float(xpos) - Input.lastX;
  GLfloat yoffset = Input.lastY - float(ypos);

  Input.lastX = float(xpos);
  Input.lastY = float(ypos);
}


void OnMouseScroll(GLFWwindow* window, double xoffset, double yoffset)
{
  // ...
}


int initGL()
{
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize OpenGL context" << std::endl;
		return -1;
	}

	std::cout << "Vendor: " << glGetString(GL_VENDOR) << std::endl;
	std::cout << "Renderer: " << glGetString(GL_RENDERER) << std::endl;
	std::cout << "Version: " << glGetString(GL_VERSION) << std::endl;
	std::cout << "GLSL: " << glGetString(GL_SHADING_LANGUAGE_VERSION) << std::endl;

  std::cout << "Controls: "<< std::endl;
  std::cout << "press right mouse button to capture/release mouse cursor  "<< std::endl;
  std::cout << "W, A, S, D - movement  "<< std::endl;
  std::cout << "E - action" << std::endl;
  std::cout << "press ESC to exit" << std::endl;

	return 0;
}

void Draw_pic(Point coord, Image &pic, Image &screen, bool treasure, int alpha){
  Pixel pix;
  for (int i = 0; i < pic.Width();i++ ){
    for (int j = 0; j < pic.Height(); j++){
      if ((pic.GetPixel(i, pic.Height() -1 - j).a !=0) || (! treasure) && (alpha == -1)){
       pix = pic.GetPixel(i, pic.Height() -1 - j);
       if (alpha != -1) pix.a = alpha;
       screen.PutPixel(coord.x + i, coord.y + j, pix);
      }
    }
  }
}


void Draw_lvl(Player &player, Image &screen, Image& borders, Image& floor,
 Image& pl, Image& exit_room, Image& EXIT, Image &back, Image& treasure, int map_i, int map_j, int &prev_move, bool first){
  char c;
  Point coords {.x = 0, .y = screen.Height()-tileSize}, temp_coords{.x = 0, .y = 0};
  int i = 0, j=0;
  c = lvl_map.get();
  while (!lvl_map.eof()){
    switch (c){
      case '#'://NUM 0 - СТЕНЫ

      Draw_pic(coords, borders, screen);
      pos_massive[(31 - i)*32 + j] = 0;
      break;

      case '.'://NUM 1 - ПОЛ

      Draw_pic(coords, floor, screen);
      pos_massive[(31 - i)*32 + j] = 1;
      break;

      case '@'://NUM 1 - ПЕРСОНАЖ

      Draw_pic(coords, floor, screen);
      player.spawn_coords = coords;
      if ((prev_move == 0) || (!first)){
      player.Coords() = coords;
      player.Old_coords() = coords;
      }
      pos_massive[(31 - i)*32 + j] = 1;
      
      break;

      case 'x'://NUM 3 - ВЫХОД ИЗ КОМНАТЫ
      if ( ((map_i == 0) && (j==0)) || ((map_i == 4)&&(j==31)) ||
        ((map_j == 0)&&(i == 0)) || ((i == 31)&&(map_j == 4)) ){
        Draw_pic(coords, borders, screen);
        pos_massive[(31 - i)*32 + j] = 0;
      }
      else{
        Draw_pic(coords, exit_room, screen);
        pos_massive[(31 - i)*32 + j] = 3;
      }

      if ((prev_move == 1) && (j==0)){
        player.Coords().x = coords.x + tileSize;
        player.Old_coords().x = coords.x + tileSize;
        player.Coords().y = coords.y;
        player.Old_coords().y = coords.y;
        prev_move = -1;
      }
      else if ((prev_move == 2)&&(i==0)){
        player.Coords().x = coords.x;
        player.Old_coords().x = coords.x;
        player.Coords().y = coords.y - tileSize;
        player.Old_coords().y = coords.y - tileSize;
        prev_move = -1;
      }
      else if ((prev_move == 3) && (j == 31)){
        player.Coords().x = coords.x - tileSize;
        player.Old_coords().x = coords.x - tileSize;
        player.Coords().y = coords.y;
        player.Old_coords().y = coords.y;
        prev_move = -1;
      }
      else if ((prev_move == 4)&&(i==31)){
        player.Coords().x = coords.x;
        player.Old_coords().x = coords.x;
        player.Coords().y = coords.y + tileSize;
        player.Old_coords().y = coords.y + tileSize;
        prev_move = -1;
      }
      break;

      case 'Q'://NUM 4 - ВЫХОД ИЗ ЛАБИРИНТА
      Draw_pic(coords, EXIT, screen);
      pos_massive[(31 - i)*32 + j] = 4;
      break;

      case ' '://NUM 5 - ПУСТОТА
      Draw_pic(coords, back, screen);
      pos_massive[(31 - i)*32 + j] = 5;
      break;

      case 'G'://NUM6 - Сокровище
      Draw_pic(coords, floor, screen);
      if ((first) || (!first && (pos_massive[(31 - i)*32 + j] == 6))){
      Draw_pic(coords, treasure, screen, true);
      pos_massive[(31 - i)*32 + j] = 6;
      }
      else {
        Draw_pic(coords, tres_taken, screen, true);
        pos_massive[(31 - i)*32 + j] = 7;
      }
      break;

      default:
      c = lvl_map.get();
      continue;
      break;
    }
    ++j;
    

    if (j % 32 == 0){
      j = 0;
      ++i;
      coords.y -= tileSize;
    } 
    coords.x = j*tileSize;

      c = lvl_map.get();
  }
  //player.Draw(screen);
  lvl_map.close();
}


void Draw_Area(Image &screen, char var, Player& player, Image &EXIT, Image &pl, int map_i, int map_j, int &prev_move, bool first){
  char c;
  Image treasure("./resources/TREASURE.png");
  switch(var){
    case 'A':
    {
    Image borders_A("./resources/ROOM_A/BORD_A.png");
    Image floor_A("./resources/ROOM_A/floor_A.png");
    Image back_A("./resources/ROOM_A/EMPTY_A.png");
    Image exit_room_A("./resources/ROOM_A/EXIT_A.png");

    lvl_map.open("./resources/ROOM_A/MAP_A.txt",std::ios::in);
    player.Floor() =  floor_A;
    Draw_lvl(player, screen, borders_A, floor_A, pl, exit_room_A, EXIT, back_A, treasure, map_i, map_j, prev_move, first);
    
    }
    break;
    case 'B':
    {
    Image borders_B("./resources/ROOM_B/BORD_B.png");
    Image floor_B("./resources/ROOM_B/floor_B.png");
    Image back_B("./resources/ROOM_B/EMPTY_B.png");
    Image exit_room_B("./resources/ROOM_B/EXIT_B.png");

    lvl_map.open("./resources/ROOM_B/MAP_B.txt",std::ios::in);
    player.Floor() = floor_B;
    Draw_lvl(player, screen, borders_B, floor_B, pl, exit_room_B, EXIT, back_B, treasure,  map_i, map_j, prev_move, first);
    
    }
    break;
    case 'C':
    {
    Image borders_C("./resources/ROOM_C/BORD_C.png");
    Image floor_C("./resources/ROOM_C/floor_C.png");
    Image back_C("./resources/ROOM_C/EMPTY_C.png");
    Image exit_room_C("./resources/ROOM_C/EXIT_C.png");

    lvl_map.open("./resources/ROOM_C/MAP_C.txt",std::ios::in);
    player.Floor() = floor_C;
    Draw_lvl(player, screen, borders_C, floor_C, pl, exit_room_C, EXIT, back_C, treasure, map_i, map_j, prev_move, first);
    
    }
    break;
    case 'D':{
    Image borders_D("./resources/ROOM_D/BORD_D.png");
    Image floor_D("./resources/ROOM_D/floor_D.png");
    Image back_D("./resources/ROOM_D/EMPTY_D.png");
    Image exit_room_D("./resources/ROOM_D/EXIT_D.png");

    lvl_map.open("./resources/ROOM_D/MAP_D.txt",std::ios::in);
    player.Floor() = floor_D;
    Draw_lvl(player, screen, borders_D, floor_D, pl, exit_room_D, EXIT, back_D, treasure, map_i, map_j, prev_move, first);
    }
    break;
    case 'F':{
    Image borders_F("./resources/ROOM_F/BORD_F.png");
    Image floor_F("./resources/ROOM_F/floor_F.png");
    Image back_F("./resources/ROOM_F/EMPTY_F.png");
    Image exit_room_F("./resources/ROOM_F/EXIT_F.png");

    lvl_map.open("./resources/ROOM_F/MAP_F.txt",std::ios::in);
    player.Floor() = floor_F;
    Draw_lvl(player, screen, borders_F, floor_F, pl, exit_room_F, EXIT, back_F, treasure, map_i, map_j, prev_move, first);
    } 
    break;
  }
  
}


void input_map(char *map){
  global_map.open("./resources/MAP.txt");
  char c;
  int i =0, j = 0;
  c = global_map.get();
  while (!global_map.eof()){
    if ((c >='A') && (c<='Z')){
    map[i + j*5] = c;
    ++i;
    if (i%5 == 0){
      ++j;
      i = 0;
      }
    }
    c = global_map.get();
  }
  global_map.close();
}

int main(int argc, char** argv)
{
	if(!glfwInit())
    return -1;

//	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
//	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
//	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

  GLFWwindow*  window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "Shrek' wonderful adventure!", nullptr, nullptr);
	if (window == nullptr)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
	
	glfwMakeContextCurrent(window); 

	glfwSetKeyCallback        (window, OnKeyboardPressed);  
	glfwSetCursorPosCallback  (window, OnMouseMove); 
  glfwSetMouseButtonCallback(window, OnMouseButtonClicked);
	glfwSetScrollCallback     (window, OnMouseScroll);

	if(initGL() != 0) 
		return -1;
	
  //Reset any OpenGL errors which could be present for some reason
	GLenum gl_error = glGetError();
	while (gl_error != GL_NO_ERROR)
		gl_error = glGetError();
  /////
	Point starting_pos{.x = 0, .y = 0};
	Player player{starting_pos};

	Image screenBuffer(WINDOW_WIDTH, WINDOW_HEIGHT, 4);


  
  player.Player_image() = pl_image;
  player.Treasure() = tres;
  player.Treasure_taken() = tres_taken;

  glViewport(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);  GL_CHECK_ERRORS;
  glClearColor(0.0f, 0.0f, 0.0f, 1.0f); GL_CHECK_ERRORS;
  //////

  
  
  input_map(G_map);
  room = G_map[map_i + map_j*5];
  Draw_Area(screenBuffer, room, player, EXIT, pl_image, map_i, map_j, prev_move);
  Draw_HP_BAR(screenBuffer, player.HPs);
  //game loop
	while (!glfwWindowShouldClose(window))
	{
		GLfloat currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;
    glfwPollEvents();

    room = processPlayerMovement(player, map_i, map_j, G_map, prev_move, screenBuffer, window);
    if ((map_i != map_i_old)||(map_j != map_j_old)){
      Draw_Area(screenBuffer, room, player, EXIT, pl_image, map_i, map_j, prev_move);
      map_i_old = map_i;
      map_j_old = map_j;

    }
    Draw_fade(tres_up, screenBuffer, x_fade, y_fade, window);

    if (finish != 'l') player.Draw(screenBuffer, pos_massive);

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); GL_CHECK_ERRORS;
    glDrawPixels (WINDOW_WIDTH, WINDOW_HEIGHT, GL_RGBA, GL_UNSIGNED_BYTE, screenBuffer.Data()); GL_CHECK_ERRORS;

		glfwSwapBuffers(window);
	}

	glfwTerminate();
	return 0;
}
