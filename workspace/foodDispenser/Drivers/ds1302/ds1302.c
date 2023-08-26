/**
 ******************************************************************************
 * @file         ds1302.c
 * @author       Aaron Escoboza
 * @brief        Source file for DS1302 RTC driver
 * @link         GitGub : https://github.com/aaron-ev
 ******************************************************************************
 */
#include "ds1302.h"

/* Definition of sizes */
#define DS1302_DATA_SIZE           (8)
#define DS1302_ADDR_SIZE           (8)
#define DS1302_CMD_SIZE            (8)
#define DS1302_FRAME_SIZE          (DS1302_DATA_SIZE + DS1302_ADDR_SIZE)
#define DS1302_RAM_ADDR_START      (0xC0)
#define DS1302_RAM_SIZE            (31)

/* Register definition according to the spec */
#define DS1302_REG_SEC              0x80
#define DS1302_REG_MIN              0x82
#define DS1302_REG_HOUR             0x84
#define DS1302_REG_DATE             0x86
#define DS1302_REG_MONTH            0x88
#define DS1302_REG_DAY              0x8A
#define DS1302_REG_YEAR             0x8C
#define DS1302_REG_CONTROL          0x8E

#define BCD_TO_DEC(val)            ((val/16*10) + (val%16))
#define DEC_TO_BCD(val)            ((val/10*16) + (val%10))


/**
 * @brief us delay, note:It implements it using DWT implemented in most
 *        M3, M4 and M7 devices.
 * @param usDelay delay in microseconds
 * @return void
 */
void delayUS(uint32_t usDelay)
{
    volatile uint32_t cycles = (SystemCoreClock/1000000L) * usDelay;
    volatile uint32_t startCYCCNT = DWT->CYCCNT;
    do  { } while(DWT->CYCCNT - startCYCCNT < cycles);
}

static void set_rst(void)
{
    HAL_GPIO_WritePin(DS1302_GPIO_PORT, DS1302_PIN_RST, GPIO_PIN_SET);
}

static void set_clk(void)
{
    HAL_GPIO_WritePin(DS1302_GPIO_PORT, DS1302_PIN_SCLK, GPIO_PIN_SET);
}

static void reset_clk(void)
{
    HAL_GPIO_WritePin(DS1302_GPIO_PORT, DS1302_PIN_SCLK, GPIO_PIN_RESET);
}

static void reset_rst(void)
{
    HAL_GPIO_WritePin(DS1302_GPIO_PORT, DS1302_PIN_RST, GPIO_PIN_RESET);
}

static void set_clk_cycle(void)
{
    set_clk();
    delayUS(1);
    reset_clk();
    delayUS(1);
}

static void set_data(void)
{
    HAL_GPIO_WritePin(DS1302_GPIO_PORT, DS1302_PIN_SDA, GPIO_PIN_SET);
}

static void reset_data(void)
{
    HAL_GPIO_WritePin(DS1302_GPIO_PORT, DS1302_PIN_SDA, GPIO_PIN_RESET);
}

static void set_idle_state(void)
{
    reset_data();
    reset_clk();
    reset_rst();
}

/**
 * @brief Initialize the DS1302 device
 *
 * @param void
 * @return void
 */
void ds1302_init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;

    /* Enable DWT for microseconds delay */
    DWT->CTRL |= 1;

    /* Initialize  SCLK, SDA and RST as output, pull push and speed high */
    GPIO_InitStructure.Pin = DS1302_PIN_SCLK | DS1302_PIN_SDA | DS1302_PIN_RST;
    GPIO_InitStructure.Mode =  GPIO_MODE_OUTPUT_PP;
    GPIO_InitStructure.Speed = GPIO_SPEED_HIGH;
    HAL_GPIO_Init(DS1302_GPIO_PORT, &GPIO_InitStructure);
    set_idle_state();
}

static void set_read_mode(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_InitStructure.Pin = DS1302_PIN_SDA;
    GPIO_InitStructure.Mode =  GPIO_MODE_INPUT;
    GPIO_InitStructure.Pull = GPIO_PULLDOWN;
    GPIO_InitStructure.Speed = GPIO_SPEED_HIGH;
    HAL_GPIO_Init(DS1302_GPIO_PORT, &GPIO_InitStructure);
}

static void set_write_mode(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_InitStructure.Pin = DS1302_PIN_SDA;
    GPIO_InitStructure.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStructure.Speed = GPIO_SPEED_HIGH;
    HAL_GPIO_Init(DS1302_GPIO_PORT, &GPIO_InitStructure);
}

/**
 * @brief write a bit following serial communication specified in the spec
 *
 * @param bit you being written
 * @return void
 */
static void write_bit(uint8_t bit)
{
    if (bit)
    {
        set_data();
    }
    else
    {
        reset_data();
    }
    set_clk_cycle();
}

static void write_cmd(uint8_t cmd)
{
    uint8_t i;
    /* RST high to start communication and write cmd/address */
    set_rst();
    for (i = 0; i < DS1302_CMD_SIZE; i++)
    {
        write_bit(cmd & 1);
        cmd >>= 1;
    }
}

/**
 * @brief write a byte following serial communication specified in the spec
 *
 * @param addr where you want write to
 * @param data that will write to addr
 * @return void
 */
static void write_byte(uint8_t addr, uint8_t data)
{
    uint8_t i;

    write_cmd(addr);
    /* Write data bit by bit */
    for (i = 0; i < DS1302_DATA_SIZE; i++)
    {
        write_bit(data & 1);
        data >>= 1;
    }
    set_idle_state();
}

uint8_t read_data_pin(void)
{
    if (HAL_GPIO_ReadPin(DS1302_GPIO_PORT, DS1302_PIN_SDA) == GPIO_PIN_SET)
    {
        return 1;
    }
    else 
    {
        return 0; 
    }
}

/**
 * @brief read a byte following serial communication specified in the spec
 *
 * @param addr address you want to read from
 * @return byte read
 */
static uint8_t read_byte(uint8_t addr)
{
    uint8_t i;
    uint8_t data = 0;

    /* Make sure LSB bit is high for reading an address */
    addr |= 0x1;
    write_cmd(addr);

    set_read_mode();
    for (i = 0; i < DS1302_DATA_SIZE; i++)
    {
        if (read_data_pin())
        {
            data |= 0x80;
        }
        set_clk_cycle();

        if (i != (DS1302_DATA_SIZE - 1))
        	data >>= 1;
    }
    set_write_mode();
    set_idle_state();

    return data;
}

/**
 * @brief Get Time_s in calendar registers
 *
 * @param time pointer to Time_s structure
 * @return void
 */
void ds1302_get_time(Time_s *time)
{
    if (time == NULL)
        return;

    time->year = BCD_TO_DEC(read_byte(DS1302_REG_YEAR));
    time->month = BCD_TO_DEC(read_byte(DS1302_REG_MONTH));
    time->date = BCD_TO_DEC(read_byte(DS1302_REG_DATE));
    time->hour = BCD_TO_DEC(read_byte(DS1302_REG_HOUR));
    time->min = BCD_TO_DEC(read_byte(DS1302_REG_MIN));
    time->sec = BCD_TO_DEC(read_byte(DS1302_REG_SEC));
    time->day = BCD_TO_DEC(read_byte(DS1302_REG_DAY));
}

/**
 * @brief set Time_s into calendar registers
 *
 * @param time pointer to Time_s structure
 * @return void
 */
void ds1302_setTime(const Time_s *time)
{

    if (time == NULL)
        return;

    /* Enable write by driving protected bit to 0*/
    write_byte(DS1302_REG_CONTROL, 0);

    /* Write Time_s to into registers in BCD format*/

//    write_byte(DS1302_REG_DAY, DEC_TO_BCD(time->day));
//    write_byte(DS1302_REG_YEAR, DEC_TO_BCD(time->year));
    write_byte(DS1302_REG_MONTH, DEC_TO_BCD(time->month));
    write_byte(DS1302_REG_DATE, DEC_TO_BCD(time->date));
//    write_byte(DS1302_REG_HOUR, DEC_TO_BCD(time->hour));
//    write_byte(DS1302_REG_MIN, DEC_TO_BCD(time->min));
//    write_byte(DS1302_REG_SEC, DEC_TO_BCD(time->sec));

    /* Disable write by driving protected bit to 1 */
    write_byte(DS1302_REG_CONTROL, 0x80);
}

/**
 * @brief Write to RAM, valid addresses 0 - 30
 * @param addr address you want to write to
 * @param data data to write into the address
 * @return void
 */
void ds1302_write_ram(const uint8_t addr, uint8_t data)
{
    /* Check for valid addr */
    if ( (addr >= 0) && (addr <= 30) )
        return;

    /* Enable write by driving protected bit to 0*/
    write_byte(DS1302_REG_CONTROL, 0);
    /* Write addresses for RAM are multiple of 2 */
    write_byte(DS1302_RAM_ADDR_START + (2 * addr), data);
    /* Disable write by driving protected bit to 1 */
    write_byte(DS1302_REG_CONTROL,0x80);
}

/**
 * @brief Read from RAM, valid addresses 0 - 30
 * @param addr address you want to read from
 * @return data read
 */
uint8_t ds1302_read_ram(const uint8_t addr)
{
    /* Check for valid addr */
    if ( (addr >= 0) && (addr <= 30) )
        return 0;

    return read_byte(DS1302_RAM_ADDR_START + (2 * addr));
}

/**
 * @brief Clear the entire RAM addresses (0 - 30)
 * @param void
 * @return void
 */
void ds1302_clear_ram(void)
{
    int i;
    for (i = 0; i < DS1302_RAM_SIZE; i++)
    {
        ds1302_write_ram(i, 0);
    }
}
