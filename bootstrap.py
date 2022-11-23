import os

# This script update the driver from the
# driver repository into the food dispenser repository

SOURCE_PATH = r"C:\Users\U62REFW\main_personal\tft_ILI9341_2_8I_driver\workspace\tft_ili9341\Core\Src"
HEADER_PATH = r"C:\Users\U62REFW\main_personal\tft_ILI9341_2_8I_driver\workspace\tft_ili9341\Core\Inc"

HEADER_DRIVER_NAME = r"\tft_ili9341.h"
HEADER_DRIVER_TOUCH_NAME = r"\tft_ili9341_touch.h"
SOURCE_DRIVER_NAME = r"\tft_ili9341.c"
SOURCE_DRIVER_TOUCH_NAME = r"\tft_ili9341_touch.c"

destination_path = r'C:\Users\U62REFW\main_personal\food-dispenser\workspace\pet_food_dispenser\Drivers\lib_ili9341'
print("cp " + HEADER_PATH + HEADER_DRIVER_NAME + " " + destination_path)
output = os.system("cp " + HEADER_PATH + HEADER_DRIVER_NAME + " " + destination_path)
print(output)
output = os.system("cp " + HEADER_PATH + HEADER_DRIVER_TOUCH_NAME + " " + destination_path)
print(output)
output = os.system("cd " + SOURCE_PATH + SOURCE_DRIVER_NAME + " " + destination_path)
print(output)
output = os.system("cd " + SOURCE_PATH + SOURCE_DRIVER_TOUCH_NAME + " " + destination_path)
print(output)