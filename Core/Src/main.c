/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2023 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "fonts.h"
#include "ST7735.h"
#include <cstring>
#include <time.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define WIDTH      					ST7735_WIDTH 						// display width in pixels
#define HEIGHT      				ST7735_HEIGHT 						// display height in pixels
#define SNAKE_SIZE  				5   						// initial snake size
#define Background_Color 		ST7735_CYAN 		//In-game background color
#define Snake_Color					ST7735_BLUE  		//In_game snake color
#define Apple_Color 				ST7735_RED			//In-game apple color
#define UP									0   						//We use 0-3 numbers for directions
#define DOWN								1
#define RIGHT								2
#define LEFT								3
#define Cube_Size						6							//cube size for snake blocks and apple (LENGTH = 16 AND WIDTH = 16)
#define UpperLimitX         22            //limits for random number generation;
#define LowerLimitX         1							//for x and y coordinates of apple;
#define UpperLimitY         27 						//
#define LowerLimitY         1							//
#define Offset							(Cube_Size/2 - 1)  //fixes the postion of boxes onscreen
//#define ScreenRefreshDelay  300;							 //to match display rate with realtime operation
//#define TextDisp						1000;
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
SPI_HandleTypeDef hspi1;

/* USER CODE BEGIN PV */
int ScreenRefreshDelay = 200;
int TextDisp = 2000;
int inMenu = 0;
int CurrentLength = SNAKE_SIZE;
int CurrentDir;
int LastX = WIDTH - Cube_Size; 			//for snake's last box X
int LastY = HEIGHT - Cube_Size;			//for snake's last box Y
uint32_t previousMillis = 0;
uint32_t currentMillis = 0;
//int Direction[4] = {0, 0, 0, 0}; //[UP, DOWN, RIGHT, LEFT]
int dir = UP; //0 for UP, 1 for DOWN, 2 for RIGHT, 3 for LEFT

struct appleLoc {  //structure for coordinates of apple
	int x;
	int y;
};

struct snake {
	int x;
	int y;
};


/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_SPI1_Init(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

struct appleLoc apple;  //initialized an appleLoc structure named apple
struct snake Snake[100]; //initialized a snake array structure named Snake with upto 100 blocks

//Function Definitions
void Menu_Init();
void Game_Init();
void Game_draw();
void New_Apple();
int Game_move();
void Loss_Message();
void Loss_Check();
void goUp();
void goDown();
void goLeft();
void goRight();


//Menu functions
void Menu_Init()
{
	inMenu = 1;
	ST7735_FillScreen(ST7735_YELLOW);
	ST7735_DrawString( WIDTH/2 - 28 , HEIGHT/2 , "PRESS UP", Font_7x10 , ST7735_RED , ST7735_YELLOW);					//Each character takes 7 pixel horizontal, 10 pixel vertical,
	ST7735_DrawString(  WIDTH/2 - 28 , HEIGHT/2 + 10 , "TO START", Font_7x10 , ST7735_RED , ST7735_YELLOW);		//Added 10 to Y axis for next line
	HAL_Delay(5);
	dir = DOWN;
	while(dir != UP)																															  
	{
		HAL_Delay(1); //Delay required to register input from button
	}
	HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_13);
	ST7735_FillScreen(ST7735_YELLOW);
	ST7735_DrawString( WIDTH/2 - 56 , HEIGHT/2 , "GAME STARTING...", Font_7x10 , ST7735_RED , ST7735_YELLOW);
	HAL_Delay(TextDisp);
}

void Game_Init()
{
	apple.x = ((rand() % (UpperLimitX - LowerLimitX + 1)) * Cube_Size) - 2;
  apple.y = ((rand() % (UpperLimitY - LowerLimitY + 1)) * Cube_Size) + 2;				
	CurrentLength = SNAKE_SIZE;			//Initial snake size is 5 blocks
	CurrentDir = UP;								//Initial snake direction
	for(int i = 0; i < CurrentLength; i++) //initialized snake blocks' spawn location
	{
		Snake[i].x = (WIDTH/2);
		Snake[i].y = (HEIGHT/2) + (Cube_Size * i);
	}		
	ST7735_FillScreen(Background_Color);  //Cleared the screen for game
	inMenu = 0;
}

void Game_draw()
{
	//Written By Muzammil
	 // Clear the screen
  //ST7735_FillScreen(Background_Color); //Edited by Faseeh (Not Needed)

	// Draw the apple
  ST7735_FillRectangle(apple.x - Offset, apple.y - Offset, Cube_Size - 1, Cube_Size - 1, Apple_Color); //Edited by Faseeh; subtracted 1 from w and h for empty space	
	
  // Draw the snake //Edited by Faseeh (drawing the boxes on new location and removing the last box)
	ST7735_FillRectangle(LastX - Offset, LastY - Offset, Cube_Size - 1, Cube_Size - 1, Background_Color);
  for (int i = 0; i < CurrentLength; i++) {
    ST7735_FillRectangle(Snake[i].x - Offset, Snake[i].y - Offset, Cube_Size - 1, Cube_Size - 1, Snake_Color);
  }

	//Edited by Faseeh (Border not needed)
  // Draw a border around the snake and apples
  //ST7735_DrawRect(Snake[0].x, Snake[0].y, Cube_Size, Cube_Size, ST7735_BLACK);
  //ST7735_DrawRect(apple.x, apple.y, Cube_Size, Cube_Size, ST7735_BLACK);
}

void New_Apple()
{
	
	//Written By Muzammil 
	// Generate random coordinates for the apple
  apple.x = ((rand() % (UpperLimitX - LowerLimitX + 1)) * Cube_Size) - 2; //Edited by Faseeh;Added limits for rand generator
  apple.y = ((rand() % (UpperLimitY - LowerLimitY + 1)) * Cube_Size) + 2;   //Edited by Faseeh;Added limits for rand generator
}


int Game_move()
{
	//Written by Faseeh
	 //Check if the snake has eaten the apple
  if(Snake[0].x == apple.x  &&  Snake[0].y == apple.y)
  {
    CurrentLength += 1;
		New_Apple();
	//	LastX = Snake[CurrentLength - 2].x;  //Save position of last box but current length increased so minus 2
	//	LastY = Snake[CurrentLength - 2].y;
  }
	
	LastX = Snake[CurrentLength - 1].x;
	LastY = Snake[CurrentLength - 1].y;
	
  
	// Update position of other snake blocks
	
	for(int i = CurrentLength - 1 ; i > 0 ; i--)
	{
		Snake[i].x = Snake[i-1].x;
		Snake[i].y = Snake[i-1].y;
	}

  // Move the snake
  if(dir == UP) 				Snake[0].y -= Cube_Size;
  else if(dir == DOWN)	Snake[0].y += Cube_Size;
  else if(dir == RIGHT)	Snake[0].x += Cube_Size;
  else if(dir == LEFT)	Snake[0].x -= Cube_Size;
	
	

  // Check if the snake has hit a wall
  //if(Snake[0].y < 0 || Snake[0].y > HEIGHT || Snake[0].x > WIDTH || Snake[0].x < 0)//If snake goes beyond screen boundary
  //{
	//	return 1;
  //  //Snake[0].y = HEIGHT - (Cube_Size);  //Send it to bottom of screen
  //}
  //if(Snake[0].y > HEIGHT)						//If snake goes below screen boundary
  //{
  //  Snake[0].y = (Cube_Size);  //Send it to top of screen
  //}
  //if(Snake[0].x > WIDTH)						//If snake goes right and out of screen boundary
  //{
  //  Snake[0].x = (Cube_Size);  //Send it to left of screen
  //}
  //if(Snake[0].x < 0)						//If snake goes left and out of screen boundary
  //{
  //  Snake[0].x = WIDTH - (Cube_Size);  //Send it to right of screen
  //}
	return 0;
}

void Loss_Message()
{
	//Written by Faseeh
	ST7735_FillScreen(ST7735_RED);
	ST7735_DrawString(WIDTH/2 - 21 , HEIGHT/2, "YOU LOST", Font_7x10, ST7735_YELLOW, ST7735_RED);
	HAL_Delay(TextDisp);
}

void Loss_Check()
{
	//Written by Faseeh
	int i;
	// Check if the snake has hit a wall
  if(Snake[0].y < 0 || Snake[0].y > HEIGHT || Snake[0].x > WIDTH || Snake[0].x < 0)
	{
		goto Loss;
	}
	//check if the snake itself
	for(i=3; i<=CurrentLength; i++)  //iterate through all blocks
	{
		if(Snake[0].x == Snake[i].x  &&  Snake[0].y == Snake[i].y)	//checking against each blocks x and y axis
		{
			Loss:
			Loss_Message();
			Menu_Init();				//Going into menu again
			Game_Init();
			//return 1;
		}
	}
	//return 0;
}

//Written By Faseeh
void goUp()
{
    if (dir == RIGHT || dir == LEFT) dir = UP; //LEFT or RIGHT, so UP
}

void goDown()
{
    if (dir == RIGHT || dir == LEFT) dir = DOWN; //LEFT or RIGHT, so DOWN
}

void goLeft()
{
    if (dir == UP || dir == DOWN) dir = LEFT; //UP or DOWN, so LEFT
}

void goRight()
{
    if (dir == UP || dir == DOWN) dir = RIGHT; //UP or DOWN, so RIGHT
}

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */
	

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_SPI1_Init();
  /* USER CODE BEGIN 2 */
	ST7735_Init();
  //ST7735_Backlight_On();
	ST7735_SetRotation(2);
	HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_13);
	Menu_Init();
	HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_13);
	Game_Init();
	HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_13);
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
		
		//ONLY FOR DEBUGGING!!!
		//ST7735_DrawLine(WIDTH/2, 0, WIDTH/2, HEIGHT, ST7735_RED);
		//ST7735_DrawLine(0, HEIGHT/2, WIDTH, HEIGHT/2, ST7735_RED);
		//ST7735_FillRectangle(WIDTH/2 - (Cube_Size/2 - 1), HEIGHT/2 - (Cube_Size/2 - 1), Cube_Size -1, Cube_Size -1, ST7735_BLUE);
		//for(i = 0; i<CurrentLength; i++)
		//{
		//	char corr = (char)(Snake[i].x) + (char)(Snake[i].y);
		//	ST7735_DrawString(WIDTH - 20, 0, &corr, Font_7x10, ST7735_RED, Background_Color);
		//}
	
		
		HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_13);
		HAL_Delay(ScreenRefreshDelay);
		
		Game_draw(); //Display the game
		
		HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_13);
		HAL_Delay(ScreenRefreshDelay);
		
		Loss_Check();
		
		HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_13);
		HAL_Delay(ScreenRefreshDelay);
		
		Game_move();	//get next move
		
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV1;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL9;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief SPI1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_SPI1_Init(void)
{

  /* USER CODE BEGIN SPI1_Init 0 */

  /* USER CODE END SPI1_Init 0 */

  /* USER CODE BEGIN SPI1_Init 1 */

  /* USER CODE END SPI1_Init 1 */
  /* SPI1 parameter configuration*/
  hspi1.Instance = SPI1;
  hspi1.Init.Mode = SPI_MODE_MASTER;
  hspi1.Init.Direction = SPI_DIRECTION_2LINES;
  hspi1.Init.DataSize = SPI_DATASIZE_8BIT;
  hspi1.Init.CLKPolarity = SPI_POLARITY_LOW;
  hspi1.Init.CLKPhase = SPI_PHASE_1EDGE;
  hspi1.Init.NSS = SPI_NSS_SOFT;
  hspi1.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_16;
  hspi1.Init.FirstBit = SPI_FIRSTBIT_MSB;
  hspi1.Init.TIMode = SPI_TIMODE_DISABLE;
  hspi1.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
  hspi1.Init.CRCPolynomial = 10;
  if (HAL_SPI_Init(&hspi1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN SPI1_Init 2 */

  /* USER CODE END SPI1_Init 2 */

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};
/* USER CODE BEGIN MX_GPIO_Init_1 */
/* USER CODE END MX_GPIO_Init_1 */

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOD_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOC, GPIO_PIN_13, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_12|GPIO_PIN_13|GPIO_PIN_14, GPIO_PIN_RESET);

  /*Configure GPIO pin : PC13 */
  GPIO_InitStruct.Pin = GPIO_PIN_13;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

  /*Configure GPIO pins : PA1 PA2 PA3 PA4 */
  GPIO_InitStruct.Pin = GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3|GPIO_PIN_4;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pins : PB12 PB13 PB14 */
  GPIO_InitStruct.Pin = GPIO_PIN_12|GPIO_PIN_13|GPIO_PIN_14;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /* EXTI interrupt init*/
  HAL_NVIC_SetPriority(EXTI1_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(EXTI1_IRQn);

  HAL_NVIC_SetPriority(EXTI2_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(EXTI2_IRQn);

  HAL_NVIC_SetPriority(EXTI3_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(EXTI3_IRQn);

  HAL_NVIC_SetPriority(EXTI4_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(EXTI4_IRQn);

/* USER CODE BEGIN MX_GPIO_Init_2 */
/* USER CODE END MX_GPIO_Init_2 */
}

/* USER CODE BEGIN 4 */

//All User-Functions Defined Here

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
  currentMillis = HAL_GetTick();
	if(inMenu == 0)
	{
		if (GPIO_Pin == GPIO_PIN_1 && (currentMillis - previousMillis > 10))
		{
			goUp(); //UP is active
			previousMillis = currentMillis;
		}
		if (GPIO_Pin == GPIO_PIN_2 && (currentMillis - previousMillis > 10))
		{
			goDown();  //Down is active
			previousMillis = currentMillis;
		}
		if (GPIO_Pin == GPIO_PIN_3 && (currentMillis - previousMillis > 10))
		{
			goRight(); //Right is active
			previousMillis = currentMillis;
		}
		if (GPIO_Pin == GPIO_PIN_4 && (currentMillis - previousMillis > 10))
		{
			goLeft();  //Left is active
			previousMillis = currentMillis;
		}
	}
	else
	{
		if (GPIO_Pin == GPIO_PIN_1 && (currentMillis - previousMillis > 10))
		{
			dir = UP;
			previousMillis = currentMillis;
		}
	}
}



/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
