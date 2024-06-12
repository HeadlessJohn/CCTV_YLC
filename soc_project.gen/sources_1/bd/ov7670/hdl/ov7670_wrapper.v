//Copyright 1986-2022 Xilinx, Inc. All Rights Reserved.
//Copyright 2022-2023 Advanced Micro Devices, Inc. All Rights Reserved.
//--------------------------------------------------------------------------------
//Tool Version: Vivado v.2023.2 (lin64) Build 4029153 Fri Oct 13 20:13:54 MDT 2023
//Date        : Fri May 31 11:46:29 2024
//Host        : Y-laptop running 64-bit Ubuntu 24.04 LTS
//Command     : generate_target ov7670_wrapper.bd
//Design      : ov7670_wrapper
//Purpose     : IP block netlist
//--------------------------------------------------------------------------------
`timescale 1 ps / 1 ps

module ov7670_wrapper
   (D_data,
    bt_uart_rxd,
    bt_uart_txd,
    control_state,
    echo,
    href,
    iic_rtl_scl_io,
    iic_rtl_sda_io,
    out_signal,
    ov7670_pwdn,
    ov7670_reset,
    ov7670_xclk,
    pclk,
    ready_display,
    reset,
    sioc_to_ov7670,
    siod_to_ov7670,
    sys_clock,
    testmode,
    trig,
    usb_uart_rxd,
    usb_uart_txd,
    vauxn15,
    vauxp15,
    vga_blue,
    vga_green,
    vga_hsync,
    vga_red,
    vga_vsync,
    vsync);
  input [7:0]D_data;
  input bt_uart_rxd;
  output bt_uart_txd;
  output [5:0]control_state;
  input echo;
  input href;
  inout iic_rtl_scl_io;
  inout iic_rtl_sda_io;
  output [3:0]out_signal;
  output ov7670_pwdn;
  output ov7670_reset;
  output ov7670_xclk;
  input pclk;
  output ready_display;
  input reset;
  output sioc_to_ov7670;
  output siod_to_ov7670;
  input sys_clock;
  input testmode;
  output trig;
  input usb_uart_rxd;
  output usb_uart_txd;
  input vauxn15;
  input vauxp15;
  output [3:0]vga_blue;
  output [3:0]vga_green;
  output vga_hsync;
  output [3:0]vga_red;
  output vga_vsync;
  input vsync;

  wire [7:0]D_data;
  wire bt_uart_rxd;
  wire bt_uart_txd;
  wire [5:0]control_state;
  wire echo;
  wire href;
  wire iic_rtl_scl_i;
  wire iic_rtl_scl_io;
  wire iic_rtl_scl_o;
  wire iic_rtl_scl_t;
  wire iic_rtl_sda_i;
  wire iic_rtl_sda_io;
  wire iic_rtl_sda_o;
  wire iic_rtl_sda_t;
  wire [3:0]out_signal;
  wire ov7670_pwdn;
  wire ov7670_reset;
  wire ov7670_xclk;
  wire pclk;
  wire ready_display;
  wire reset;
  wire sioc_to_ov7670;
  wire siod_to_ov7670;
  wire sys_clock;
  wire testmode;
  wire trig;
  wire usb_uart_rxd;
  wire usb_uart_txd;
  wire vauxn15;
  wire vauxp15;
  wire [3:0]vga_blue;
  wire [3:0]vga_green;
  wire vga_hsync;
  wire [3:0]vga_red;
  wire vga_vsync;
  wire vsync;

  IOBUF iic_rtl_scl_iobuf
       (.I(iic_rtl_scl_o),
        .IO(iic_rtl_scl_io),
        .O(iic_rtl_scl_i),
        .T(iic_rtl_scl_t));
  IOBUF iic_rtl_sda_iobuf
       (.I(iic_rtl_sda_o),
        .IO(iic_rtl_sda_io),
        .O(iic_rtl_sda_i),
        .T(iic_rtl_sda_t));
  ov7670 ov7670_i
       (.D_data(D_data),
        .bt_uart_rxd(bt_uart_rxd),
        .bt_uart_txd(bt_uart_txd),
        .control_state(control_state),
        .echo(echo),
        .href(href),
        .iic_rtl_scl_i(iic_rtl_scl_i),
        .iic_rtl_scl_o(iic_rtl_scl_o),
        .iic_rtl_scl_t(iic_rtl_scl_t),
        .iic_rtl_sda_i(iic_rtl_sda_i),
        .iic_rtl_sda_o(iic_rtl_sda_o),
        .iic_rtl_sda_t(iic_rtl_sda_t),
        .out_signal(out_signal),
        .ov7670_pwdn(ov7670_pwdn),
        .ov7670_reset(ov7670_reset),
        .ov7670_xclk(ov7670_xclk),
        .pclk(pclk),
        .ready_display(ready_display),
        .reset(reset),
        .sioc_to_ov7670(sioc_to_ov7670),
        .siod_to_ov7670(siod_to_ov7670),
        .sys_clock(sys_clock),
        .testmode(testmode),
        .trig(trig),
        .usb_uart_rxd(usb_uart_rxd),
        .usb_uart_txd(usb_uart_txd),
        .vauxn15(vauxn15),
        .vauxp15(vauxp15),
        .vga_blue(vga_blue),
        .vga_green(vga_green),
        .vga_hsync(vga_hsync),
        .vga_red(vga_red),
        .vga_vsync(vga_vsync),
        .vsync(vsync));
endmodule
