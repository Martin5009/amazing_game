`default_nettype wire
/*
displaydriver_top
	Top level module with display driver and clock divider.
*/
module displaydriver_top (input	logic clk,
						input	logic reset,
						input	logic sck,
						input	logic sdi,
						input	logic cen,
						output	logic done,
						output logic csclk,
						output logic data,
						output logic wr,
						output logic cs,
						output [3:0] led);
	
	logic [4:0] counter;
	logic divclk;
	
	// Incoming MCU clock is 16 MHz. Display runs at 256 kHz.
	// Clock must be divided to avoid sending data to display faster than it can write.
	always_ff @(posedge clk) begin
		if (~reset) counter <= 0;
		else	   counter <= counter + 2'b1;
	end

	assign divclk = counter[4];
	
	displaydriver dd(divclk, reset, sck, sdi, cen, done, csclk, data, wr, cs, led);
	
endmodule
					 
/*
displaydriver
	Top level module for display driver. Included SPI interface.
*/
module displaydriver (input	logic clk,
					 input	logic reset,
					 input	logic sck,
					 input	logic sdi,
					 input	logic cen,
					 output	logic done,
					 output logic csclk,
					 output logic data,
					 output logic wr,
					 output logic cs,
					 output [3:0] led);
	
	logic [15:0] msg;
	logic [13:0] chpdata;
	logic [10:0] shftcnt, shftval;
	logic [2:0] chpnum;
	logic csrst, wrreset, init, sdo, wrpulse, shftset, nxtchp, wden, dclk;
	logic ncs, csd;
	
	logic counter;
	
	// Counter
	always_ff @(posedge clk) begin
		if (~reset) counter <= 0;
		else	   counter <= counter + 2'b1;
	end
	
	assign dclk = counter;
	
	spireceiver		spi(sck, sdi, cen, sdo, msg);
	decoder			dc(msg, chpnum, chpdata, shftval);
	writedisplay	wd(dclk, reset, ~wrreset, wden, init, chpdata, wr, wrpulse, data);
	controller		cn(dclk, ~reset, cen, shftcnt, chpnum, done, init, nxtchp, cs, shftset, csrst, wrreset, wden, led);
	dwncntr			dcnt(dclk, wrpulse, shftset, shftval, shftcnt);
	upcntr			ucnt(dclk, nxtchp, ~csrst, chpnum);
	assign csclk = nxtchp;
	
endmodule

/*
hsosc
	Module for the built-in hi-speed oscillator on the FPGA.
 	Unused, as MCU sends clock to FPGA.
*/
module hsosc (input		logic reset,
			  output	logic clk);
	
	logic [4:0] counter;
	
	// Internal high-speed oscillator
	HSOSC #(.CLKHF_DIV(2'b01)) 
		hf_osc (.CLKHFPU(1'b1), .CLKHFEN(1'b1), .CLKHF(osc));

	// Counter
	always_ff @(posedge osc) begin
		if (~reset) counter <= 0;
		else	   counter <= counter + 2'b1;
	end

	assign clk = counter[4];

endmodule
