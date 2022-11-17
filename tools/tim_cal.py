timx_clk = 16000000; # Hz
prescaler = 0;
cnt_clk = timx_clk/(prescaler + 1) #Timer count clock
time_base = 100 / 1000
max_16bit_dec_value = 65535;
period = time_base / (1 / cnt_clk);

print("Debug: prescaler = {}, cnt_clk = {}, period = {}" .format(prescaler, cnt_clk, period))
while (period > max_16bit_dec_value or not(str(period).endswith('.0'))):
    prescaler = prescaler + 1
    cnt_clk = timx_clk / (prescaler + 1) #Timer count clock
    period = time_base / (1 / cnt_clk);
    print("Debug: prescaler = {}, cnt_clk = {}, period = {}" .format(prescaler, cnt_clk, period))

# print results
print("Results: prescaler = {}, cnt_clk = {}, period = {}" .format(prescaler, cnt_clk, period))