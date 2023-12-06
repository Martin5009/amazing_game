`default_nettype wire

/*
shftreg
	When init is asserted, stores d then shifts it out on the clk when en is asserted.
*/
module shftreg #(parameter N=14)
			  (input	logic clk,
			   input	logic reset,
			   input	logic en,
			   input	logic init,
			   input	logic [N-1:0] d,
			   output	logic sdo);
	
	logic [N-1:0] d1;
	logic enpulse;
	
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
			S0:		if 		(en) 	  nextstate = S1;
					else			  nextstate = S0;
			S1:		if 		(en)	  nextstate = S2;
					else			  nextstate = S0;
			S2:		if		(en)	  nextstate = S2;
					else			  nextstate = S0;
			default: 				  nextstate = S0;
		endcase
		
	// output logic
	assign enpulse = (state == S1);
	
	always_ff @(negedge clk)
		if (~init)	begin
			sdo = 0;
			d1 = d;
		end
		else if	(enpulse) 	begin
			{sdo, d1[N-1:1]} = d1;
		end
endmodule
