module spireceiver #(parameter N=16)
				  (input	logic sck,
				   input	logic sdi,
				   input	logic cen,
				   output	logic sdo,
				   output	logic [N-1:0] msg);
	
	logic wasdone;
	
	always_ff @(posedge sck)
        if (cen)  {msg} = {msg[N-2:0], sdi};
	
endmodule