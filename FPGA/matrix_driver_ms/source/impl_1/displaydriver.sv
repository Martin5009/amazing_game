`default_nettype wire
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
	
	// Counter
	always_ff @(posedge clk) begin
		if (~reset) counter <= 0;
		else	   counter <= counter + 2'b1;
	end

	assign divclk = counter[4];
	
	displaydriver dd(divclk, reset, sck, sdi, cen, done, csclk, data, wr, cs, led);
	
endmodule
					 

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