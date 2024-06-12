`timescale 1ns / 1ps

module step_motor #(
    parameter SYS_CLK   = 100    // 100MHz 
    )(
    input clk,
    input reset_p,
    input [11:0] deg_target, // 목표 각도 입력
    input fast_mode,         // 0 : 1 step,  1 : 2 step
    input set_center,        // 현재 위치한 각도를 가운데(2048)로 설정
    input [31:0] step_time,         // 최소 1.8ms 이상 (fast_mode =0이면 900ms 이상)
    input motor_on,          // 모터 동작 여부
    output reg [11:0] deg_cnt,
    output [3:0] out_signal
    );

    //    0.087890625 deg :    1 step
    //    0.17578125  deg :    2 step
    //    0.3515625   deg :    4 step
    //    0.703125    deg :    8 step
    //    1.40625     deg :   16 step
    //    2.8125      deg :   32 step
    //    5.625       deg :   64 step
    //   11.25        deg :  128 step
    //   22.5         deg :  256 step
    //   45           deg :  512 step
    //   90           deg : 1024 step
    //  180           deg : 2048 step
    //  360           deg : 4096 step

    localparam signal1 = 8'b1100_0001;
    localparam signal2 = 8'b0111_0000;
    localparam signal3 = 8'b0001_1100;
    localparam signal4 = 8'b0000_0111;

    reg [2:0] n;
    wire motor_en;
    wire motor_dir;

    wire clk_us;
    clock_usec #(SYS_CLK) clock_usec_inst (.clk        (clk),
                                           .reset_p    (reset_p),
                                           .clock_usec (clk_us)    );
    
    reg [15:0] us_cnt;
    reg cp_step;
    always @(posedge clk, posedge reset_p) begin
        if (reset_p) begin
            us_cnt <= 0;
            cp_step <= 0;
        end
        else begin
            if (clk_us) begin
                if (us_cnt < step_time) begin
                    us_cnt <= us_cnt + 1;
                    cp_step <= 0;
                end
                else begin
                    us_cnt <= 0;
                    cp_step <= 1;
                end
            end
            else begin
                cp_step <= 0;
            end
        end
    end

    always @(posedge clk, posedge reset_p) begin
        if(reset_p) begin
            n          <= 3'b0;
            deg_cnt    <= 12'b0;
        end 
        else begin
            if (motor_on && motor_en && cp_step) begin // 2ms 
                if (motor_dir) begin
                    if (fast_mode == 0) begin
                        n <= n + 1; // n : 0~7; 
                        deg_cnt <= deg_cnt + 1;
                    end
                    else begin
                        n <= n + 2; // n : 0~7; 
                        deg_cnt <= deg_cnt + 2;
                    end
                end
                else begin
                    if (fast_mode == 0) begin
                        n <= n - 1; // n : 7~0; 
                        deg_cnt <= deg_cnt - 1;
                    end
                    else begin
                        n <= n - 2; // n : 7~0; 
                        deg_cnt <= deg_cnt - 2;
                    end
                end
            end

            if (set_center) begin // set_center 신호를 받으면 현재 상태를 가운데(2048)로 설정
                n <= 3'b0;
                deg_cnt <= 12'd2048;
            end
        end
    end
    
    assign motor_en = (deg_target != deg_cnt); // target과 현재 각도가 같으면 동작 멈춤
    assign motor_dir = deg_target > deg_cnt; // 목표 각도가 더 크면 시계방향으로 회전
    assign out_signal = motor_en ? { signal1[n], signal2[n], signal3[n], signal4[n] } : 4'b0000; // 목표 도달시 전류 차단

endmodule
