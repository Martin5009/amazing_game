/*
upcntr
	Standard enable and resettable up-counter.
*/
module upcntr #(parameter N=3)
			  (input	logic clk,
			   input	logic en,
			   input	logic reset,
			   output	logic [N-1:0] cnt);
			   
	always_ff @(posedge clk) begin
		if (~reset) 	cnt <= 0;
		else if (en)	cnt <= cnt + 1'd1;
	end
	
endmodule

/*
dwncntr
	Settable down-counter.
*/
module dwncntr #(parameter N=11)
			  (input	logic clk,
			   input	logic en,
			   input	logic set,
			   input	logic [10:0] setval,
			   output	logic [N-1:0] cnt);
			   
	always_ff @(posedge clk) begin
		if (set) 				cnt <= setval;
		else if (en && cnt>0)	cnt <= cnt - 1'd1;
	end
	
endmodule

/*
ffenr
	Flip-flop with reset and enable.
*/
module ffenr #(parameter N=1)
			  (input	logic clk,
			   input	logic en,
			   input	logic reset,
			   input	logic [N-1:0] d,
			   output	logic [N-1:0] q);
	
	always_ff @(posedge clk)
		if 		(~reset)	q<=0;
		else if	(en)		q<=d;
			   
endmodule
