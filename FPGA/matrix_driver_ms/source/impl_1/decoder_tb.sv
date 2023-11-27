`timescale 1ns/1ns
`default_nettype none
`define N_TV 8

module decoder_tb();
 // Set up test signals
 logic [13:0] chpdata;
 logic [15:0] msg;
 logic [10:0] shftval;
 logic [2:0] chpnum;
 logic clk;

 // Instantiate the device under test
 decoder dut(msg, chpnum, chpdata, shftval);

 // Generate clock signal with a period of 10 timesteps.
 always
   begin
     clk = 1; #5;
     clk = 0; #5;
   end

 initial begin
 #2;
 msg = 16'b0_0001_0000_01_00001;
 chpnum = 3'b001; #10;
 chpnum = 3'b010; #10;
 chpnum = 3'b011; #10;
 chpnum = 3'b100; #10;

 $stop;
 end

endmodule
