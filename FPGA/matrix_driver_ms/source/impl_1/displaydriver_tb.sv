`timescale 1ns/1ns
`default_nettype none
`define N_TV 8

module displaydriver_tb #(parameter N=8)();
 // Set up test signals
 logic clk, wr, cen, sdi, done, reset, cs, data, csclk;

 // Instantiate the device under test

 displaydriver	dd(clk, reset, clk, sdi, cen, done, csclk, data, wr, cs);

 // Generate clock signal with a period of 10 timesteps.
 always
   begin
     clk = 1; #5;
     clk = 0; #5;
   end

 initial begin
 #2;
 reset = 0; cen = 0; #10; reset = 1;#10;
 cen = 1; #10; 
 #5; sdi = 1'b1;
 #10; sdi = 1'b0;
 #10; sdi = 1'b0;
 #10; sdi = 1'b0;
 #10; sdi = 1'b0;
 #10; sdi = 1'b0;
 #10; sdi = 1'b0;
 #10; sdi = 1'b0; 
 #10; sdi = 1'b1;
 #10; sdi = 1'b0;
 #10; sdi = 1'b0;
 #10; sdi = 1'b0;
 #10; sdi = 1'b0;
 #10; sdi = 1'b0;
 #10; sdi = 1'b0;
 #10; sdi = 1'b0;
 #5; cen = 0;
 #20000;

 $stop;
 end

endmodule
