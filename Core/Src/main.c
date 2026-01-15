#include "main.h"


SPI_HandleTypeDef hspi1;


void SystemClock_Config(void);

static void MX_GPIO_Init(void);

static void MX_SPI1_Init(void);


uint8_t screen[8];

int bird_y = 0;

int velocity = 0;

uint32_t lastPress = 0;

int pipe_x = 7;

int gap_y = 3;


void newPipe()

{

    pipe_x = 7;

    gap_y = (HAL_GetTick() % 5) + 1;  // gap between rows 1–5

}


void drawPipe()

{

    for(int y = 0; y < 8; y++)

    {

        if(y != gap_y && y != gap_y + 1)

            screen[y] |= (1 << pipe_x);

    }

}


void updateBird()

{

    velocity += 1;

    bird_y += velocity;


    if(bird_y < 0) bird_y = 0;

    if(bird_y > 7) bird_y = 7;

}


void clearScreen()

{

    for(int i = 0; i < 8; i++)

        screen[i] = 0;

}


void MAX7219_Send(uint8_t addr, uint8_t data)

{

    uint8_t tx[2] = {addr, data};


    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, GPIO_PIN_RESET); // CS LOW

    HAL_SPI_Transmit(&hspi1, tx, 2, 10);

    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, GPIO_PIN_SET);   // CS HIGH

}


void drawScreen()

{

    for(int row = 0; row < 8; row++)

    {

        MAX7219_Send(row + 1, screen[row]);

    }

}



int main(void)

{


  HAL_Init();


  SystemClock_Config();


  MX_GPIO_Init();

  MX_SPI1_Init();

  MAX7219_Send(0x0F, 0x00);

  MAX7219_Send(0x0C, 0x01);

  MAX7219_Send(0x0B, 0x07);

  MAX7219_Send(0x0A, 0x08);

  MAX7219_Send(0x09, 0x00);



  while (1)

  {

	  clearScreen();


	  if(HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_0) == GPIO_PIN_RESET)

	  {

	      if(HAL_GetTick() - lastPress > 200)

	      {

	          velocity = -3;

	          lastPress = HAL_GetTick();

	      }

	  }


	  updateBird();


	  pipe_x--;

	  if(pipe_x < 0)

	      newPipe();


	  drawPipe();


	  /* collision */

	  if(pipe_x == 1)

	  {

	      if(bird_y != gap_y && bird_y != gap_y + 1)

	      {

	          while(1)

	          {

	              for(int i = 1; i <= 8; i++)

	                  MAX7219_Send(i, 0xAA);

	              HAL_Delay(300);

	              for(int i = 1; i <= 8; i++)

	                  MAX7219_Send(i, 0x55);

	              HAL_Delay(300);

	          }

	      }

	  }


	  screen[bird_y] |= (1 << 1);


	  drawScreen();


	  HAL_Delay(120);





  }

}



void SystemClock_Config(void)

{

  RCC_OscInitTypeDef RCC_OscInitStruct = {0};

  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};



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


static void MX_SPI1_Init(void)

{


  hspi1.Instance = SPI1;

  hspi1.Init.Mode = SPI_MODE_MASTER;

  hspi1.Init.Direction = SPI_DIRECTION_2LINES;

  hspi1.Init.DataSize = SPI_DATASIZE_8BIT;

  hspi1.Init.CLKPolarity = SPI_POLARITY_LOW;

  hspi1.Init.CLKPhase = SPI_PHASE_1EDGE;

  hspi1.Init.NSS = SPI_NSS_SOFT;

  hspi1.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_32;

  hspi1.Init.FirstBit = SPI_FIRSTBIT_MSB;

  hspi1.Init.TIMode = SPI_TIMODE_DISABLE;

  hspi1.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;

  hspi1.Init.CRCPolynomial = 10;

  if (HAL_SPI_Init(&hspi1) != HAL_OK)

  {

    Error_Handler();

  }



}



static void MX_GPIO_Init(void)

{

  GPIO_InitTypeDef GPIO_InitStruct = {0};



  /* GPIO Ports Clock Enable */

  __HAL_RCC_GPIOD_CLK_ENABLE();

  __HAL_RCC_GPIOA_CLK_ENABLE();


  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, GPIO_PIN_RESET);


  GPIO_InitStruct.Pin = GPIO_PIN_4;

  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;

  GPIO_InitStruct.Pull = GPIO_NOPULL;

  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;

  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);




  __HAL_RCC_GPIOB_CLK_ENABLE();


  GPIO_InitStruct.Pin = GPIO_PIN_0;

  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;

  GPIO_InitStruct.Pull = GPIO_PULLUP;

  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);



}



void Error_Handler(void)

{


  __disable_irq();

  while (1)

  {

  }


}

#ifdef USE_FULL_ASSERT


void assert_failed(uint8_t *file, uint32_t line)

{


}

#endif

