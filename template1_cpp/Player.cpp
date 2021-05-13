#include "Player.h"
#include "Image.h"
#include <fstream>
#include <GLFW/glfw3.h>
#include <iostream>
bool Player::Moved() const
{
  if(coords.x == old_coords.x && coords.y == old_coords.y)
    return false;
  else
    return true;
}

char Player::ProcessInput(MovementDir dir, int lim, int *pos_massive,  int &map_i,  int &map_j, int &prev_move )
{
  int move_dist = move_speed * 1;
  int i_mas = coords.x/32, j_mas = coords.y/32,
   i_mas_up = (coords.x + tileSize -1)/32, j_mas_up = (coords.y + tileSize-1 + move_dist)/32; 
   int i_move_right = (coords.x + tileSize -1 + move_dist)/32, j_move_right = (coords.y + tileSize - 1)/32;
   int i_move_down = (coords.x + tileSize - 1)/32, j_move_down = (coords.y - move_dist)/32;
   int i_move_left = (coords.x - move_dist)/32, j_move_left = (coords.y + tileSize -1)/32;

  switch(dir)
  {
    case MovementDir::UP:
    if (old_coords.y + move_dist + tileSize -1 <=lim){

    	if ((pos_massive[i_mas + j_mas_up*32] == 0) || (pos_massive[i_mas_up + j_mas_up*32]==0)){
    		for (int i = move_speed; i>0; --i){
    			if ((pos_massive[i_mas + (coords.y + tileSize -1 + i)/32 * 32] == 1) && (pos_massive[i_mas_up + (coords.y + tileSize -1 + i)/32 * 32] == 1)){
    				old_coords.y = coords.y;
    				coords.y += i;
    				break;
    			}
    		}
    	}
    	else if ((pos_massive[i_mas + j_mas_up*32] == 1) && (pos_massive[i_mas_up + j_mas_up*32]==1) ||
    	(pos_massive[i_mas + j_mas_up*32] == 6) || (pos_massive[i_mas_up + j_mas_up*32]==6) ||
    	(pos_massive[i_mas + j_mas_up*32] == 7) || (pos_massive[i_mas_up + j_mas_up*32]==7)) {
    			old_coords.y = coords.y;
    			coords.y += move_dist;
    		}
    	else if ((pos_massive[i_mas + j_mas_up*32] == 3) && (pos_massive[i_mas_up + j_mas_up*32]==3)){
    		if (map_j == 0) {
    			std::cout <<"Выход в пустоту!"<<std::endl;
    			glfwTerminate();
    		}
    		else {
    			--map_j;
    			prev_move = 4;
    			
    		}
    	}
    	else if ((pos_massive[i_mas + j_mas_up*32] == 4) && (pos_massive[i_mas_up + j_mas_up*32]==4)){
    		return 'f';
    	}
    	else if ((pos_massive[i_mas + j_mas_up*32] == 5) && (pos_massive[i_mas_up + j_mas_up*32]==5)){
    		--HPs;
    		if (HPs > 0){ 
    			return 'r';
    			} 
    		else return 'l';
    	}
    }
    break;

    case MovementDir::DOWN:
    if (old_coords.y - move_dist >=0){
      
    if ((pos_massive[i_mas + j_move_down*32] == 0) || (pos_massive[i_move_down + j_move_down*32]==0)){
    		for (int i = move_speed; i>0; --i){
    			if ((pos_massive[i_mas + (coords.y - i)/32 * 32] == 1) && (pos_massive[i_move_down + (coords.y - i)/32 * 32] == 1)){
    				old_coords.y = coords.y;
    				coords.y -= i;
    				break;
    			}
    		}
    }
    else if ((pos_massive[i_mas + j_move_down*32] == 1) && (pos_massive[i_move_down + j_move_down*32]==1) || 
    	(pos_massive[i_mas + j_move_down*32] == 6) || (pos_massive[i_move_down + j_move_down*32]==6) || 
    	(pos_massive[i_mas + j_move_down*32] == 7) || (pos_massive[i_move_down + j_move_down*32]==7)){
    	old_coords.y = coords.y;
    	coords.y -= move_dist;
    }
    else if ((pos_massive[i_mas + j_move_down*32] == 3) && (pos_massive[i_move_down + j_move_down*32]==3)){
    	if (map_j == 4) {
    			std::cout <<"Выход в пустоту!"<<std::endl;
    			glfwTerminate();
    		}
    		else {
    			++map_j;
    			prev_move = 2;
    			
    		}
    }
    else if ((pos_massive[i_mas + j_move_down*32] == 4) && (pos_massive[i_move_down + j_move_down*32]==4)){
    	return 'f';
    }
    else if ((pos_massive[i_mas + j_move_down*32] == 5) && (pos_massive[i_move_down + j_move_down*32]==5)){
    		--HPs;
    		if (HPs > 0) {
    			return 'r'; 
    		}
    		else return 'l';
    }


 	 }	
    break;


    case MovementDir::LEFT:
    if (old_coords.x - move_dist >=0){
      
      if ((pos_massive[i_move_left + j_mas*32] == 0) || (pos_massive[i_move_left + j_move_left*32]==0)){
    		for (int i = move_speed; i>0; --i){
    			if ((pos_massive[(coords.x - i)/32 + 32 * j_mas] == 1) && (pos_massive[(coords.x - i)/32 + j_move_left*32] == 1)){
    				old_coords.x = coords.x;
    				coords.x -= i;
    				break;
    			}
    		}
    	}
    	else if((pos_massive[i_move_left + j_mas*32] == 1) && (pos_massive[i_move_left + j_move_left*32]==1) || 
    		(pos_massive[i_move_left + j_mas*32] == 6) || (pos_massive[i_move_left + j_move_left*32]==6) ||
    		(pos_massive[i_move_left + j_mas*32] == 7) || (pos_massive[i_move_left + j_move_left*32]==7)){
    		old_coords.x = coords.x;
    		coords.x -= move_dist;
    	}
    	else if((pos_massive[i_move_left + j_mas*32] == 3) && (pos_massive[i_move_left + j_move_left*32]==3)){
    		if (map_i == 0) {
    			std::cout <<"Выход в пустоту!"<<std::endl;
    			glfwTerminate();
    		}
    		else {
    			--map_i;
    			prev_move = 3;
    			
    		}
    	}
    	else if((pos_massive[i_move_left + j_mas*32] == 4) && (pos_massive[i_move_left + j_move_left*32]==4)){
    		return 'f';
    	}
    	else if((pos_massive[i_move_left + j_mas*32] == 5) && (pos_massive[i_move_left+ j_move_left*32]==5)){
    		--HPs;
    		if (HPs > 0) {
    			return 'r'; 
    		}
    		else return 'l';
    	}

  	}

    break;

    case MovementDir::RIGHT:
    if(old_coords.x + move_dist + tileSize -1 <=lim){

      if ((pos_massive[i_move_right + j_mas*32] == 0) || (pos_massive[i_move_right + j_move_right*32]==0)){
    		for (int i = move_speed; i>0; --i){
    			if ((pos_massive[(coords.x + i + tileSize -1)/32 + 32 * j_mas] == 1) && (pos_massive[(coords.x + i + tileSize - 1)/32 + j_move_right*32] == 1)){
    				old_coords.x = coords.x;
    				coords.x += i;
    				break;
    			}
    		}
    	}
    	else if((pos_massive[i_move_right + j_mas*32] == 1) && (pos_massive[i_move_right+ j_move_right*32]==1) || 
    		(pos_massive[i_move_right + j_mas*32] == 6) || (pos_massive[i_move_right+ j_move_right*32]==6) ||
    		(pos_massive[i_move_right + j_mas*32] == 7) || (pos_massive[i_move_right+ j_move_right*32]==7)){
    		old_coords.x = coords.x;
    		coords.x += move_dist;
    	}
    	else if((pos_massive[i_move_right + j_mas*32] == 3) && (pos_massive[i_move_right + j_move_right*32]==3)){
    		if (map_i == 4) {
    			std::cout <<"Выход в пустоту!"<<std::endl;
    			glfwTerminate();
    		}
    		else {
    			++map_i;
    			prev_move = 1;
    			
    		}
    	}
    	else if((pos_massive[i_move_right + j_mas*32] == 4) && (pos_massive[i_move_right + j_move_right*32]==4)){
    		return 'f';
    	}
    	else if((pos_massive[i_move_right + j_mas*32] == 5) && (pos_massive[i_move_right + j_move_right*32]==5)){
    		--HPs;
    		if (HPs > 0){
    		 return 'r'; 
    		}
    		else return 'l';
    	}

  	}
    break;

    default:
    break;
	}
	return 'n';
}

void Player::Draw(Image &screen, int* pos_massive)
{
  if(Moved())
  {
    	for(int y = old_coords.y; y <= old_coords.y + tileSize-1; ++y)
    	{
      	for(int x = old_coords.x; x <= old_coords.x + tileSize -1; ++x)
      	{
      		if (pos_massive[x/32 + 32*(y/32)]==6){
      			if (treasure.GetPixel((x%tileSize), treasure.Height() - 1 - (y%tileSize)).a !=0)
      				screen.PutPixel(x, y, treasure.GetPixel((x%tileSize), treasure.Height() - 1 - (y%tileSize)));
      			else screen.PutPixel(x, y, floor.GetPixel((x%tileSize), floor.Height() - 1 - (y%tileSize)));
      		} 
      		else if (pos_massive[x/32 + 32*(y/32)]==7){
      			if (treasure_taken.GetPixel((x%tileSize), treasure_taken.Height() - 1 - (y%tileSize)).a !=0)
      			screen.PutPixel(x, y, treasure_taken.GetPixel((x%tileSize), treasure_taken.Height() - 1 - (y%tileSize)));
      			else screen.PutPixel(x, y, floor.GetPixel((x%tileSize), floor.Height() - 1 - (y%tileSize)));
      		}
      		
      		else screen.PutPixel(x, y, floor.GetPixel((x%tileSize), floor.Height() - 1 - (y%tileSize)));
      	}
    	}
    	old_coords = coords;
  }

  for(int y = coords.y; y < coords.y + tileSize; ++y)
  {
    for(int x = coords.x; x < coords.x + tileSize; ++x)
    {
      if (player_image.GetPixel(x - coords.x, player_image.Height() - 1 - (y - coords.y)).a != 0) 
      	screen.PutPixel(x, y, player_image.GetPixel(x - coords.x, player_image.Height() - 1 - (y - coords.y)));
    }
  }
}

