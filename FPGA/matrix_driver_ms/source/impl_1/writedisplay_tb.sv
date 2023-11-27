`timescale 1ns/1ns
`default_nettype none
`define N_TV 8

module writedisplay_tb();
 // Set up test signals
 logic clk, wr, wrpulse, cs, sdo, reset, init;
 logic [27:0] data;

 // Instantiate the device under test
 writedisplay dut(clk, reset, cs, init, data, wr, wrpulse, sdo);

 // Generate clock signal with a period of 10 timesteps.
 always
   begin
     clk = 1; #5;
     clk = 0; #5;
   end

 initial begin
 #2;
 reset = 0; #10; reset = 1; #10;
 data = 4'b1110; cs = 1'b0;

 #10
 //cs = 1'b1;
 data = 4'b0000;
 #1000;

 $stop;
 end

endmodule
