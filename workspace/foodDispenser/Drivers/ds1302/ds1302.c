/**
 ******************************************************************************
 * @file         ds1302.c
 * @author       Aaron Escoboza
 * @brief        Source file for DS1302 RTC driver
 * @link         GitGub : https://github.com/aaron-ev
 ******************************************************************************
 */
#include "ds1302.h"

/* Size definitions */
#define DS1302_DATA_SIZE            8
#define DS1302_CMD_SIZE             8
#define DS1302_RAM_ADDR_START       0xC0

/* Register definition according to the spec */
#define DS1302_REG_SEC              0x80
#define DS1302_REG_MIN              0x82
#define DS1302_REG_HOUR             0x84
#define DS1302_REG_DATE             0x86
#define DS1302_REG_MONTH            0x88
#define DS1302_REG_DAY              0x8A
#define DS1302_REG_YEAR             0x8C
#define DS1302_REG_CONTROL          0x8E
#define D1302_RAM_BURST_MODE        0xFE
#define D1302_CAL_BURST_MODE        0xBE /* Burst mode for calendar */

/* Macros for handling BCD format data stored in the RTC device */
#define BCD_TO_DEC(val)             ((val / 16 * 10) + (val % 16))
#define DEC_TO_BCD(val)             ((val / 10 * 16) + (val % 10))

/* Masks to get fields in the calendar*/
#define MASK_CLOCK_SYSTEM           0x80
#define MASK_CLOCK_PERIOD           0x20
#define MASK_HOURS_24               0x3F
#define MASK_HOURS_12               0x1F
#define MASK_SECONDS                0x7F

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
    do {} while(DWT->CYCCNT - startCYCCNT < cycles);
}

/**
 * @brief write a high value on RST line
 *
 * @param void
 * @return void
 */
static void set_rst(void)
{
    HAL_GPIO_WritePin(DS1302_GPIO_PORT, DS1302_PIN_RST, GPIO_PIN_SET);
}

/**
 * @brief write a high value on CLK line
 *
 * @param void
 * @return void
 */
static void set_clk(void)
{
    HAL_GPIO_WritePin(DS1302_GPIO_PORT, DS1302_PIN_SCLK, GPIO_PIN_SET);
}

/**
 * @brief write a low value on CLK line
 *
 * @param void
 * @return void
 */
static void reset_clk(void)
{
    HAL_GPIO_WritePin(DS1302_GPIO_PORT, DS1302_PIN_SCLK, GPIO_PIN_RESET);
}

/**
 * @brief write a low value on RST line
 *
 * @param void
 * @return void
 */
static void reset_rst(void)
{
    HAL_GPIO_WritePin(DS1302_GPIO_PORT, DS1302_PIN_RST, GPIO_PIN_RESET);
}

/**
 * @brief Clock cycle on CLK line
 * @param void
 * @return void
 */
static void set_clk_cycle(void)
{
    set_clk();
    delayUS(1);
    reset_clk();
    delayUS(1);
}

/**
 * @brief Write a high value on DTA line
 * @param void
 * @return void
 */
static void set_data(void)
{
    HAL_GPIO_WritePin(DS1302_GPIO_PORT, DS1302_PIN_SDA, GPIO_PIN_SET);
}

/**
 * @brief Write a low value on DTA line
 * @param void
 * @return void
 */
static void reset_data(void)
{
    HAL_GPIO_WritePin(DS1302_GPIO_PORT, DS1302_PIN_SDA, GPIO_PIN_RESET);
}

/**
 * @brief Set idle state SDA, CLK and RST low value.
 * @param void
 * @return void
 */
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
   CoreDebug->DEMCR |= CoreDebug_DEMCR_TRCENA_Msk;
   DWT->CTRL |= 1 ;
   DWT->CYCCNT = 0;

    /* Initialize  SCLK, SDA and RST as output, pull push and speed high */
    GPIO_InitStructure.Pin = DS1302_PIN_SCLK | DS1302_PIN_SDA | DS1302_PIN_RST;
    GPIO_InitStructure.Speed = GPIO_SPEED_HIGH;
    GPIO_InitStructure.Mode =  GPIO_MODE_OUTPUT_PP;
    HAL_GPIO_Init(DS1302_GPIO_PORT, &GPIO_InitStructure);

    set_idle_state();
}

/**
 * @brief Read mode: SDA pin is set as input
 * @param void
 * @return void
 */
static void set_read_mode(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_InitStructure.Pin = DS1302_PIN_SDA;
    GPIO_InitStructure.Pull = GPIO_PULLDOWN;
    GPIO_InitStructure.Speed = GPIO_SPEED_HIGH;
    GPIO_InitStructure.Mode =  GPIO_MODE_INPUT;
    HAL_GPIO_Init(DS1302_GPIO_PORT, &GPIO_InitStructure);
}

/**
 * @brief Write mode: SDA pin is set as ouput
 * @param void
 * @return void
 */
static void set_write_mode(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_InitStructure.Pin = DS1302_PIN_SDA;
    GPIO_InitStructure.Speed = GPIO_SPEED_HIGH;
    GPIO_InitStructure.Mode = GPIO_MODE_OUTPUT_PP;
    HAL_GPIO_Init(DS1302_GPIO_PORT, &GPIO_InitStructure);
}

/**
 * @brief write a command/address on SDA line
 *
 * @param bit you being written
 * @return void
 */
static void write_cmd(uint8_t cmd)
{
    uint8_t i;
    /* RST high to start communication and write cmd/address */
    set_rst();
    for (i = 0; i < DS1302_CMD_SIZE; i++)
    {
        (cmd & 1) ? set_data() : reset_data();
        set_clk_cycle();
        cmd >>= 1;
    }
}

/**
 * @brief write a byte following serial communication specified in the spec
 *
 * @param addr where you want write to
 * @param data that will write to address
 * @return void
 */
static void write_data(uint8_t addr, uint8_t data)
{
    uint8_t i;

    write_cmd(addr);
    /* Write data bit by bit */
    for (i = 0; i < DS1302_DATA_SIZE; i++)
    {
        (data & 1) ? set_data() : reset_data();
        set_clk_cycle();
        data >>= 1;
    }
    set_idle_state();
}

static GPIO_PinState read_data_pin(void)
{
    return HAL_GPIO_ReadPin(DS1302_GPIO_PORT, DS1302_PIN_SDA);
}

/**
 * @brief read a byte following serial communication specified in the spec
 *
 * @param addr address you want to read from
 * @return byte read
 */
static uint8_t read_data(uint8_t addr)
{
    uint8_t i;
    uint8_t data = 0;

    /* Make sure LSB bit is high for reading an address */
    addr |= 0x1;
    write_cmd(addr);

    set_read_mode();
    for (i = 0; i < DS1302_DATA_SIZE; i++)
    {
        /* Ones come from the MSB to LSB
         * by shifting data to the right
        */
        if (read_data_pin() == GPIO_PIN_SET)
        {
            data |= 0x80;
        }
        set_clk_cycle();
        if (i != (DS1302_DATA_SIZE - 1))
        {
            data >>= 1;
        }
    }
    set_write_mode();

    set_idle_state();

    return data;
}

/**
 * @brief Get time from calendar registers
 *
 * @param time pointer to time structure
 * @return void
 */
void ds1302_get_time(Time_s *time)
{
    uint8_t tmpMaskClockSystem = 0;

    if (time == NULL)
    {
        return;
    }
    tmpMaskClockSystem = (time->clockSystem == DS1302_CLK_SYSTEM_24) ?
                         MASK_HOURS_24: MASK_HOURS_12;
    time->min = BCD_TO_DEC(read_data(DS1302_REG_MIN));
    time->day = BCD_TO_DEC(read_data(DS1302_REG_DAY));
    time->year = BCD_TO_DEC(read_data(DS1302_REG_YEAR));
    time->date = BCD_TO_DEC(read_data(DS1302_REG_DATE));
    time->month = BCD_TO_DEC(read_data(DS1302_REG_MONTH));
    time->sec = BCD_TO_DEC((read_data(DS1302_REG_SEC) & MASK_SECONDS));
    time->hour = BCD_TO_DEC((read_data(DS1302_REG_HOUR) & tmpMaskClockSystem));
}

/**
 * @brief set time into calendar registers
 *        
 * @param time pointer to time structure
 * @return void
 */
void ds1302_set_time(const Time_s *time)
{
    uint8_t tmpMaskClockSystem = 0;

    if (time == NULL)
    {
        return;
    }

    /* When 12 clock system is set, bit 7 should be high 
     * according to the spec, low for 24 clock system.
    */
    if (time->clockSystem != DS1302_CLK_SYSTEM_24)
    {
        /* Set 12 hour clock system */
        tmpMaskClockSystem |=  MASK_CLOCK_SYSTEM;
        if (time->clockPeriod == DS1302_CLK_PM_PERIOD)
        {
            /* Set PM clock period */
            tmpMaskClockSystem |= MASK_CLOCK_PERIOD;
        }
    }
    /* Enable write by driving protected bit to 0*/
    write_data(DS1302_REG_CONTROL, 0);

    /* Write time data to RTC calendar registers in BCD format */
    write_data(DS1302_REG_MIN, DEC_TO_BCD(time->min));
    write_data(DS1302_REG_SEC, DEC_TO_BCD(time->sec));
    write_data(DS1302_REG_DAY, DEC_TO_BCD(time->day));
    write_data(DS1302_REG_YEAR, DEC_TO_BCD(time->year));
    write_data(DS1302_REG_DATE, DEC_TO_BCD(time->date));
    write_data(DS1302_REG_MONTH, DEC_TO_BCD(time->month));
    write_data(DS1302_REG_HOUR, DEC_TO_BCD(time->hour) | tmpMaskClockSystem);

    /* Disable write by driving protected bit to 1 */
    write_data(DS1302_REG_CONTROL, 0x80);
}

/**
 * @brief Write to RAM, valid addresses 0 - 30
 * @param addr address you want to write to
 * @param data data to write into the address
 * @return void
 */
void ds1302_write_ram(const uint8_t addr, uint8_t data)
{
    if (addr > (DS1302_RAM_SIZE - 1))
    { 
        return;
    }
    /* Enable write by driving protected bit to 0*/
    write_data(DS1302_REG_CONTROL, 0);
    /* Write addresses for RAM are multiple of 2 */
    write_data(DS1302_RAM_ADDR_START + (2 * addr), data);
    /* Disable write by driving protected bit to 1 */
    write_data(DS1302_REG_CONTROL, 0x80);
}

/**
 * @brief Read from RAM, valid addresses 0 - 30
 * @param addr address you want to read from
 * @return data read
 */
uint8_t ds1302_read_ram(const uint8_t addr)
{
    if (addr > (DS1302_RAM_SIZE - 1))
    {
        return 0;
    }
    return read_data(DS1302_RAM_ADDR_START + (2 * addr));
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

/**
 * @brief get clock system 12 or 24 format
 * @param void
 * @return ClockSystem 
 */
ClockSystem ds1302_get_clock_system(void)
{
    return (read_data(DS1302_REG_HOUR) & MASK_CLOCK_SYSTEM) ?
            DS1302_CLK_SYSTEM_12 : DS1302_CLK_SYSTEM_24;
}

/**
 * @brief get clock period AM or PM
 * @param void
 * @return ClockPeriod 
 */
ClockPeriod ds1302_get_clock_period(void)
{
    return (read_data(DS1302_REG_HOUR) & MASK_CLOCK_PERIOD) ?
           DS1302_CLK_PM_PERIOD : DS1302_CLK_AM_PERIOD;
}

/**
 * @brief read in burst mode
 *
 * @param addr address for ram or calendar
 * @param len  number of bytes to read
 * @param buff buffer where data will be stored
 * @return void
 */
static void read_burst(uint8_t addr, uint8_t len, uint8_t *buff)
{
    uint8_t i, j;

    /* Make sure LSB bit is high for reading an address */
    addr |= 0x1;
    write_cmd(addr);

    set_read_mode();
    for (i = 0; i < len; i++)
    {
        buff[i] = 0;
        for (j = 0; j < DS1302_DATA_SIZE; j++)
        {
            /* Ones come from the MSB to LSB
            * by shifting data to the right
            */
            if (read_data_pin() == GPIO_PIN_SET)
            {
                buff[i] |= 0x80;
            }
            set_clk_cycle();
            if (j != (DS1302_DATA_SIZE - 1))
            {
                buff[i] >>= 1;
            }
        }
    }
    set_write_mode();
    set_idle_state();
}

/**
 * @brief write in burst mode
 *
 * @param addr address for ram or calendar
 * @param len number of addresses to write to
 * @param buff buffer that will write to RTC
 * @return void
 */
static void write_burst(uint8_t addr, uint8_t len, uint8_t *buff)
{
    uint8_t i, j;

    write_cmd(addr);
    /* Write data bit by bit */
    for (i = 0; i < len; i++)
    {
        for (j = 0; j < DS1302_DATA_SIZE; j++)
        {
            (buff[i] & 1) ? set_data() : reset_data();
            set_clk_cycle();
            buff[i] >>= 1;
        }
    }
    set_idle_state();
}

/**
 * @brief Write to RAM in burst mode
 * @param len Number of addresses to write to 
 * @param buff Buffer that will write to RTC
 * @return void
 */
void ds1302_write_ram_burst(uint8_t len, uint8_t *buff)
{
    if (len > DS1302_RAM_SIZE)
    { 
        return;
    }
    /* Enable write by driving protected bit to 0*/
    write_data(DS1302_REG_CONTROL, 0);
    /* Write addresses for RAM are multiple of 2 */
    write_burst(D1302_RAM_BURST_MODE, len, buff);
    /* Disable write by driving protected bit to 1 */
    write_data(DS1302_REG_CONTROL, 0x80);
}

/**
 * @brief Read RAM in burst mode
 * @param len Number of addresses to read from RTC
 * @param buff Buffer where data will be stored
 */
void ds1302_read_ram_burst(uint8_t len, uint8_t *buff)
{
    if (len > DS1302_RAM_SIZE)
    {
        return;
    }
    memset(buff, 0, 1);
    read_burst(D1302_RAM_BURST_MODE, len, buff);
}

/**
 * @brief Get time from calendar registers in burst mode
 * @param time pointer to time structure
 * @return void
 */
void ds1302_get_time_burst(Time_s *time)
{
    /* In burst mode, 8 bytes needs to be read. Byte 8 is for control reg */
    uint8_t tmpBuff[8] = {0, 0 , 0, 0, 0, 0 ,0 , 0};

    if (time == NULL)
    {
        return;
    }
    read_burst(D1302_CAL_BURST_MODE, 8, tmpBuff);
    time->sec = BCD_TO_DEC(tmpBuff[0] & MASK_SECONDS);
    time->min = BCD_TO_DEC(tmpBuff[1]);
    time->date = BCD_TO_DEC(tmpBuff[3]);
    time->month = BCD_TO_DEC(tmpBuff[4]);
    time->day = BCD_TO_DEC(tmpBuff[5]);
    time->year = BCD_TO_DEC(tmpBuff[6]);

    /* Handle hour register */
    time->hour = (tmpBuff[2] & MASK_CLOCK_SYSTEM) ?
                 BCD_TO_DEC(tmpBuff[2] & MASK_HOURS_12):
                 BCD_TO_DEC(tmpBuff[2] & MASK_HOURS_24);
    time->clockSystem = (tmpBuff[2] & MASK_CLOCK_SYSTEM) ? DS1302_CLK_SYSTEM_12:
                                                           DS1302_CLK_SYSTEM_24;
    time->clockPeriod = (tmpBuff[2] & MASK_CLOCK_PERIOD) ? DS1302_CLK_PM_PERIOD:
                                                           DS1302_CLK_AM_PERIOD;
}

/**
 * @brief Set time into calendar registers in burst mode
 * @param time pointer to time structure
 * @return void
 */
void ds1302_set_time_burst(Time_s *time)
{
    /* In burst mode, 8 bytes needs to be read. Byte 8 is for control reg */
    uint8_t tmpBuff[8];

    if (time == NULL)
    {
        return;
    }

    tmpBuff[0] = time->sec;
    tmpBuff[1] = time->min;
    /* Handle hour reg */
    if (time->clockSystem == DS1302_CLK_SYSTEM_24)
    {
        time->hour |= MASK_CLOCK_SYSTEM;
        if (time->clockPeriod == DS1302_CLK_PM_PERIOD)
        {
            time->hour |= MASK_CLOCK_PERIOD;
        }
        else
        {
            time->hour &= ~(MASK_CLOCK_PERIOD);
        }
    }
    tmpBuff[2] = time->hour;
    tmpBuff[3] = time->date;
    tmpBuff[4] = time->month;
    tmpBuff[5] = time->day;
    tmpBuff[6] = time->year;
    tmpBuff[7] = 0;
    write_burst(D1302_CAL_BURST_MODE, 8, tmpBuff);
}
