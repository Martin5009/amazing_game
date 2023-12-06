module writedisplay (input	logic clk,
					input	logic reset,
					input	logic wrreset,
					input	logic cs,
					input	logic init,
					input	logic [13:0] chpdata,
					output	logic wr,
					output	logic wrpulse,
					output	logic sdo);
	
	logic [1:0] cnt;
	
	typedef enum logic [2:0] {S0, S1, S2} statetype;
	statetype	state, nextstate;
	
	// state register
	always_ff @(posedge clk) begin
		if (~reset)	state <= S0;
		else		state <= nextstate;
	end
	
	// next state logic
	always_comb
		case (state)
			S0:		if 		(wr) 	  nextstate = S1;
					else			  nextstate = S0;
			S1:		if 		(wr)	  nextstate = S2;
					else			  nextstate = S0;
			S2:		if		(wr)	  nextstate = S2;
					else			  nextstate = S0;
			default: 				  nextstate = S0;
		endcase
		
	// output logic
	assign wrpulse = (state == S1);
	
	upcntr #(.N(2)) wrcnt(clk, ~cs, wrreset, cnt);
	
	assign wr = ~cnt[1];
	
	shftreg #(.N(14)) sr(clk, reset, cnt[1], init, chpdata, sdo);
					
endmodule
					
