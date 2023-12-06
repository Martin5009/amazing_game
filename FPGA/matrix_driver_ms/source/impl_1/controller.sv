/*
controller
	Moore FSM controller for DE-DP14112 LED matrix decoder.
*/
module controller (input	logic clk,
				  input		logic reset,
				  input		logic cen,
				  input		logic [10:0] shftcnt,
				  input		logic [2:0]	 chpnum,
				  output	logic done,
				  output	logic init,
				  output	logic nxtchp,
				  output	logic cs,
				  output	logic shftset,
				  output	logic csrst,
				  output	logic wrreset,
				  output	logic wden,
				  output	logic [3:0] led);
	
	typedef enum logic [4:0] {S0, S1, S2, S3, S4, S5, S6, S7, S8, S9, S10, S11} statetype;
	statetype	state, nextstate;
	
	// state register
	always_ff @(posedge clk) begin
		if (reset)	state <= S0;
		else		state <= nextstate;
	end
	
	// next state logic
	always_comb
		case (state)
			S0:		if		(cen)  			nextstate = S1;
					else			    	nextstate = S0;
			S1:		if		(~cen)			nextstate = S2;
					else					nextstate = S1;
			S2:								nextstate = S3;
			S3:								nextstate = S4;
			S4:		if		(shftcnt==0)	nextstate = S9;
					else					nextstate = S4;
			S9:								nextstate = S10;
			S10:							nextstate = S5;
			S5:		if		(chpnum==4)		nextstate = S8;
					else					nextstate = S6;
			S6:								nextstate = S7;
			S7:		if		(shftcnt==0)	nextstate = S9;
					else					nextstate = S7;
			S8:		if		(cen)			nextstate = S1;
					else					nextstate = S8;
			default: 				    	nextstate = S0;
		endcase
		
	// output logic
	assign done = (state==S8);
	assign init = (state==S4)||(state==S7)||(state==S9)||(state==S10);
	assign nxtchp = (state==S2)||(state==S5);
	assign cs = ~((state==S2));
	assign shftset = (state==S3)||(state==S6);
	assign csrst = (state==S0)||(state==S8);
	assign wrreset = (state==S2)||(state==S9);
	assign wden = ~((state==S4)||(state==S7));
	assign led = state;

endmodule
