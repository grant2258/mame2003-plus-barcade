/***************************************************************************

  jackal.c

  Written by Kenneth Lin (kenneth_lin@ai.vancouver.bc.ca)

Notes:
- This game uses two 005885 gfx chip in parallel. The unique thing about it is
  that the two 4bpp tilemaps from the two chips are merged to form a single
  8bpp tilemap.
- topgunbl is derived from a completely different version, which supports gun
  turret rotation. The copyright year is also deiffrent, but this doesn't
  necessarily mean anything.

TODO:
- The high score table colors are wrong, are there proms missing?
- Sprite lag
- Coin counters don't work correctly, because the register is overwritten by
  other routines and the coin counter bits rapidly toggle between 0 and 1.

***************************************************************************/

#include "driver.h"
#include "vidhrdw/generic.h"
#include "cpu/m6809/m6809.h"


extern unsigned char *jackal_videoctrl;

MACHINE_INIT( jackal );
VIDEO_START( jackal );

READ_HANDLER( jackal_zram_r );
READ_HANDLER( jackal_commonram_r );
READ_HANDLER( jackal_commonram1_r );
READ_HANDLER( jackal_voram_r );
READ_HANDLER( jackal_spriteram_r );

WRITE_HANDLER( jackal_rambank_w );
WRITE_HANDLER( jackal_zram_w );
WRITE_HANDLER( jackal_commonram_w );
WRITE_HANDLER( jackal_commonram1_w );
WRITE_HANDLER( jackal_voram_w );
WRITE_HANDLER( jackal_spriteram_w );

PALETTE_INIT( jackal );
VIDEO_UPDATE( jackal );



static READ_HANDLER( rotary_0_r )
{
	return (1 << (readinputport(5) * 8 / 256)) ^ 0xff;
}

static READ_HANDLER( rotary_1_r )
{
	return (1 << (readinputport(6) * 8 / 256)) ^ 0xff;
}

static int irq_enable;

static WRITE_HANDLER( ctrl_w )
{
	irq_enable = data & 0x02;
	flip_screen_set(data & 0x08);
}

INTERRUPT_GEN( jackal_interrupt )
{
	if (irq_enable)
		cpu_set_irq_line(0, 0, HOLD_LINE);
}



static MEMORY_READ_START( jackal_readmem )
	{ 0x0010, 0x0010, input_port_0_r },
	{ 0x0011, 0x0011, input_port_1_r },
	{ 0x0012, 0x0012, input_port_2_r },
	{ 0x0013, 0x0013, input_port_3_r },
	{ 0x0014, 0x0014, rotary_0_r },
	{ 0x0015, 0x0015, rotary_1_r },
	{ 0x0018, 0x0018, input_port_4_r },
	{ 0x0020, 0x005f, jackal_zram_r },	/* MAIN   Z RAM,SUB    Z RAM */
	{ 0x0060, 0x1fff, jackal_commonram_r },	/* M COMMON RAM,S COMMON RAM */
	{ 0x2000, 0x2fff, jackal_voram_r },	/* MAIN V O RAM,SUB  V O RAM */
	{ 0x3000, 0x3fff, jackal_spriteram_r },	/* MAIN V O RAM,SUB  V O RAM */
	{ 0x4000, 0xbfff, MRA_BANK1 },
	{ 0xc000, 0xffff, MRA_ROM },
MEMORY_END

static MEMORY_WRITE_START( jackal_writemem )
	{ 0x0000, 0x0003, MWA_RAM, &jackal_videoctrl },	/* scroll + other things */
	{ 0x0004, 0x0004, ctrl_w },
	{ 0x0019, 0x0019, MWA_NOP },	/* possibly watchdog reset */
	{ 0x001c, 0x001c, jackal_rambank_w },
	{ 0x0020, 0x005f, jackal_zram_w },
	{ 0x0060, 0x1fff, jackal_commonram_w },
	{ 0x2000, 0x2fff, jackal_voram_w },
	{ 0x3000, 0x3fff, jackal_spriteram_w },
	{ 0x4000, 0xffff, MWA_ROM },
MEMORY_END

static MEMORY_READ_START( jackal_sound_readmem )
	{ 0x2001, 0x2001, YM2151_status_port_0_r },
	{ 0x4000, 0x43ff, MRA_RAM },		/* COLOR RAM (Self test only check 0x4000-0x423f */
	{ 0x6000, 0x605f, MRA_RAM },		/* SOUND RAM (Self test check 0x6000-605f, 0x7c00-0x7fff */
	{ 0x6060, 0x7fff, jackal_commonram1_r }, /* COMMON RAM */
	{ 0x8000, 0xffff, MRA_ROM },
MEMORY_END

static MEMORY_WRITE_START( jackal_sound_writemem )
	{ 0x2000, 0x2000, YM2151_register_port_0_w },
	{ 0x2001, 0x2001, YM2151_data_port_0_w },
	{ 0x4000, 0x43ff, paletteram_xBBBBBGGGGGRRRRR_w, &paletteram },
	{ 0x6000, 0x605f, MWA_RAM },
	{ 0x6060, 0x7fff, jackal_commonram1_w },
	{ 0x8000, 0xffff, MWA_ROM },
MEMORY_END



INPUT_PORTS_START( jackal )
	PORT_START	/* DSW1 */
	PORT_DIPNAME( 0x0f, 0x0f, DEF_STR( Coin_A ) )
	PORT_DIPSETTING(    0x02, DEF_STR( 4C_1C ) )
	PORT_DIPSETTING(    0x05, DEF_STR( 3C_1C ) )
	PORT_DIPSETTING(    0x08, DEF_STR( 2C_1C ) )
	PORT_DIPSETTING(    0x04, DEF_STR( 3C_2C ) )
	PORT_DIPSETTING(    0x01, DEF_STR( 4C_3C ) )
	PORT_DIPSETTING(    0x0f, DEF_STR( 1C_1C ) )
	PORT_DIPSETTING(    0x03, DEF_STR( 3C_4C ) )
	PORT_DIPSETTING(    0x07, DEF_STR( 2C_3C ) )
	PORT_DIPSETTING(    0x0e, DEF_STR( 1C_2C ) )
	PORT_DIPSETTING(    0x06, DEF_STR( 2C_5C ) )
	PORT_DIPSETTING(    0x0d, DEF_STR( 1C_3C ) )
	PORT_DIPSETTING(    0x0c, DEF_STR( 1C_4C ) )
	PORT_DIPSETTING(    0x0b, DEF_STR( 1C_5C ) )
	PORT_DIPSETTING(    0x0a, DEF_STR( 1C_6C ) )
	PORT_DIPSETTING(    0x09, DEF_STR( 1C_7C ) )
	PORT_DIPSETTING(    0x00, DEF_STR( Free_Play ) )
	PORT_DIPNAME( 0xf0, 0xf0, DEF_STR( Coin_B ) )
	PORT_DIPSETTING(    0x20, DEF_STR( 4C_1C ) )
	PORT_DIPSETTING(    0x50, DEF_STR( 3C_1C ) )
	PORT_DIPSETTING(    0x80, DEF_STR( 2C_1C ) )
	PORT_DIPSETTING(    0x40, DEF_STR( 3C_2C ) )
	PORT_DIPSETTING(    0x10, DEF_STR( 4C_3C ) )
	PORT_DIPSETTING(    0xf0, DEF_STR( 1C_1C ) )
	PORT_DIPSETTING(    0x30, DEF_STR( 3C_4C ) )
	PORT_DIPSETTING(    0x70, DEF_STR( 2C_3C ) )
	PORT_DIPSETTING(    0xe0, DEF_STR( 1C_2C ) )
	PORT_DIPSETTING(    0x60, DEF_STR( 2C_5C ) )
	PORT_DIPSETTING(    0xd0, DEF_STR( 1C_3C ) )
	PORT_DIPSETTING(    0xc0, DEF_STR( 1C_4C ) )
	PORT_DIPSETTING(    0xb0, DEF_STR( 1C_5C ) )
	PORT_DIPSETTING(    0xa0, DEF_STR( 1C_6C ) )
	PORT_DIPSETTING(    0x90, DEF_STR( 1C_7C ) )
	PORT_DIPSETTING(    0x00, "Invalid" )

	PORT_START	/* IN1 */
	/* note that button 3 for player 1 and 2 are exchanged */
	PORT_BIT( 0x01, IP_ACTIVE_LOW, IPT_JOYSTICK_LEFT  | IPF_8WAY | IPF_PLAYER1 )
	PORT_BIT( 0x02, IP_ACTIVE_LOW, IPT_JOYSTICK_RIGHT | IPF_8WAY | IPF_PLAYER1 )
	PORT_BIT( 0x04, IP_ACTIVE_LOW, IPT_JOYSTICK_UP    | IPF_8WAY | IPF_PLAYER1 )
	PORT_BIT( 0x08, IP_ACTIVE_LOW, IPT_JOYSTICK_DOWN  | IPF_8WAY | IPF_PLAYER1 )
	PORT_BIT( 0x10, IP_ACTIVE_LOW, IPT_BUTTON1 | IPF_PLAYER1 )
	PORT_BIT( 0x20, IP_ACTIVE_LOW, IPT_BUTTON2 | IPF_PLAYER1 )
	PORT_BIT( 0x40, IP_ACTIVE_LOW, IPT_UNKNOWN )
	PORT_BIT( 0x80, IP_ACTIVE_LOW, IPT_UNKNOWN )

	PORT_START	/* IN2 */
	PORT_BIT( 0x01, IP_ACTIVE_LOW, IPT_JOYSTICK_LEFT  | IPF_8WAY | IPF_PLAYER2 )
	PORT_BIT( 0x02, IP_ACTIVE_LOW, IPT_JOYSTICK_RIGHT | IPF_8WAY | IPF_PLAYER2 )
	PORT_BIT( 0x04, IP_ACTIVE_LOW, IPT_JOYSTICK_UP    | IPF_8WAY | IPF_PLAYER2 )
	PORT_BIT( 0x08, IP_ACTIVE_LOW, IPT_JOYSTICK_DOWN  | IPF_8WAY | IPF_PLAYER2 )
	PORT_BIT( 0x10, IP_ACTIVE_LOW, IPT_BUTTON1 | IPF_PLAYER2 )
	PORT_BIT( 0x20, IP_ACTIVE_LOW, IPT_BUTTON2 | IPF_PLAYER2 )
	PORT_BIT( 0x40, IP_ACTIVE_LOW, IPT_UNKNOWN )
	PORT_BIT( 0x80, IP_ACTIVE_LOW, IPT_UNKNOWN )

	PORT_START	/* IN0 */
	PORT_BIT( 0x01, IP_ACTIVE_LOW, IPT_COIN1 )
	PORT_BIT( 0x02, IP_ACTIVE_LOW, IPT_COIN2 )
	PORT_BIT( 0x04, IP_ACTIVE_LOW, IPT_SERVICE1 )
	PORT_BIT( 0x08, IP_ACTIVE_LOW, IPT_START1 )
	PORT_BIT( 0x10, IP_ACTIVE_LOW, IPT_START2 )
	PORT_DIPNAME( 0x20, 0x20, DEF_STR( Flip_Screen ) )
	PORT_DIPSETTING(    0x20, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_BIT( 0x40, IP_ACTIVE_LOW, IPT_UNKNOWN )
	PORT_BIT( 0x80, IP_ACTIVE_LOW, IPT_UNKNOWN )

	PORT_START	/* DSW2 */
	PORT_DIPNAME( 0x03, 0x02, DEF_STR( Lives ) )
	PORT_DIPSETTING(    0x03, "2" )
	PORT_DIPSETTING(    0x02, "3" )
	PORT_DIPSETTING(    0x01, "4" )
	PORT_DIPSETTING(    0x00, "7" )
	PORT_DIPNAME( 0x04, 0x04, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x04, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x18, 0x18, DEF_STR( Bonus_Life ) )
	PORT_DIPSETTING(    0x18, "30000 150000" )
	PORT_DIPSETTING(    0x10, "50000 200000" )
	PORT_DIPSETTING(    0x08, "30000" )
	PORT_DIPSETTING(    0x00, "50000" )
	PORT_DIPNAME( 0x60, 0x60, DEF_STR( Difficulty ) )
	PORT_DIPSETTING(    0x60, "Easy" )
	PORT_DIPSETTING(    0x40, "Medium" )
	PORT_DIPSETTING(    0x20, "Hard" )
	PORT_DIPSETTING(    0x00, "Hardest" )
	PORT_DIPNAME( 0x80, 0x00, DEF_STR( Demo_Sounds ) )
	PORT_DIPSETTING(    0x80, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
INPUT_PORTS_END

/* identical, plus additional rotary controls */
INPUT_PORTS_START( topgunbl )
	PORT_START	/* DSW1 */
	PORT_DIPNAME( 0x0f, 0x0f, DEF_STR( Coin_A ) )
	PORT_DIPSETTING(    0x02, DEF_STR( 4C_1C ) )
	PORT_DIPSETTING(    0x05, DEF_STR( 3C_1C ) )
	PORT_DIPSETTING(    0x08, DEF_STR( 2C_1C ) )
	PORT_DIPSETTING(    0x04, DEF_STR( 3C_2C ) )
	PORT_DIPSETTING(    0x01, DEF_STR( 4C_3C ) )
	PORT_DIPSETTING(    0x0f, DEF_STR( 1C_1C ) )
	PORT_DIPSETTING(    0x03, DEF_STR( 3C_4C ) )
	PORT_DIPSETTING(    0x07, DEF_STR( 2C_3C ) )
	PORT_DIPSETTING(    0x0e, DEF_STR( 1C_2C ) )
	PORT_DIPSETTING(    0x06, DEF_STR( 2C_5C ) )
	PORT_DIPSETTING(    0x0d, DEF_STR( 1C_3C ) )
	PORT_DIPSETTING(    0x0c, DEF_STR( 1C_4C ) )
	PORT_DIPSETTING(    0x0b, DEF_STR( 1C_5C ) )
	PORT_DIPSETTING(    0x0a, DEF_STR( 1C_6C ) )
	PORT_DIPSETTING(    0x09, DEF_STR( 1C_7C ) )
	PORT_DIPSETTING(    0x00, DEF_STR( Free_Play ) )
	PORT_DIPNAME( 0xf0, 0xf0, DEF_STR( Coin_B ) )
	PORT_DIPSETTING(    0x20, DEF_STR( 4C_1C ) )
	PORT_DIPSETTING(    0x50, DEF_STR( 3C_1C ) )
	PORT_DIPSETTING(    0x80, DEF_STR( 2C_1C ) )
	PORT_DIPSETTING(    0x40, DEF_STR( 3C_2C ) )
	PORT_DIPSETTING(    0x10, DEF_STR( 4C_3C ) )
	PORT_DIPSETTING(    0xf0, DEF_STR( 1C_1C ) )
	PORT_DIPSETTING(    0x30, DEF_STR( 3C_4C ) )
	PORT_DIPSETTING(    0x70, DEF_STR( 2C_3C ) )
	PORT_DIPSETTING(    0xe0, DEF_STR( 1C_2C ) )
	PORT_DIPSETTING(    0x60, DEF_STR( 2C_5C ) )
	PORT_DIPSETTING(    0xd0, DEF_STR( 1C_3C ) )
	PORT_DIPSETTING(    0xc0, DEF_STR( 1C_4C ) )
	PORT_DIPSETTING(    0xb0, DEF_STR( 1C_5C ) )
	PORT_DIPSETTING(    0xa0, DEF_STR( 1C_6C ) )
	PORT_DIPSETTING(    0x90, DEF_STR( 1C_7C ) )
	PORT_DIPSETTING(    0x00, "Invalid" )

	PORT_START	/* IN1 */
	/* note that button 3 for player 1 and 2 are exchanged */
	PORT_BIT( 0x01, IP_ACTIVE_LOW, IPT_JOYSTICK_LEFT  | IPF_8WAY | IPF_PLAYER1 )
	PORT_BIT( 0x02, IP_ACTIVE_LOW, IPT_JOYSTICK_RIGHT | IPF_8WAY | IPF_PLAYER1 )
	PORT_BIT( 0x04, IP_ACTIVE_LOW, IPT_JOYSTICK_UP    | IPF_8WAY | IPF_PLAYER1 )
	PORT_BIT( 0x08, IP_ACTIVE_LOW, IPT_JOYSTICK_DOWN  | IPF_8WAY | IPF_PLAYER1 )
	PORT_BIT( 0x10, IP_ACTIVE_LOW, IPT_BUTTON1 | IPF_PLAYER1 )
	PORT_BIT( 0x20, IP_ACTIVE_LOW, IPT_BUTTON2 | IPF_PLAYER1 )
	PORT_BIT( 0x40, IP_ACTIVE_LOW, IPT_UNKNOWN )
	PORT_BIT( 0x80, IP_ACTIVE_LOW, IPT_UNKNOWN )

	PORT_START	/* IN2 */
	PORT_BIT( 0x01, IP_ACTIVE_LOW, IPT_JOYSTICK_LEFT  | IPF_8WAY | IPF_PLAYER2 )
	PORT_BIT( 0x02, IP_ACTIVE_LOW, IPT_JOYSTICK_RIGHT | IPF_8WAY | IPF_PLAYER2 )
	PORT_BIT( 0x04, IP_ACTIVE_LOW, IPT_JOYSTICK_UP    | IPF_8WAY | IPF_PLAYER2 )
	PORT_BIT( 0x08, IP_ACTIVE_LOW, IPT_JOYSTICK_DOWN  | IPF_8WAY | IPF_PLAYER2 )
	PORT_BIT( 0x10, IP_ACTIVE_LOW, IPT_BUTTON1 | IPF_PLAYER2 )
	PORT_BIT( 0x20, IP_ACTIVE_LOW, IPT_BUTTON2 | IPF_PLAYER2 )
	PORT_BIT( 0x40, IP_ACTIVE_LOW, IPT_UNKNOWN )
	PORT_BIT( 0x80, IP_ACTIVE_LOW, IPT_UNKNOWN )

	PORT_START	/* IN0 */
	PORT_BIT( 0x01, IP_ACTIVE_LOW, IPT_COIN1 )
	PORT_BIT( 0x02, IP_ACTIVE_LOW, IPT_COIN2 )
	PORT_BIT( 0x04, IP_ACTIVE_LOW, IPT_SERVICE1 )
	PORT_BIT( 0x08, IP_ACTIVE_LOW, IPT_START1 )
	PORT_BIT( 0x10, IP_ACTIVE_LOW, IPT_START2 )
	PORT_DIPNAME( 0x20, 0x20, DEF_STR( Flip_Screen ) )
	PORT_DIPSETTING(    0x20, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_BIT( 0x40, IP_ACTIVE_LOW, IPT_UNKNOWN )
	PORT_BIT( 0x80, IP_ACTIVE_LOW, IPT_UNKNOWN )

	PORT_START	/* DSW2 */
	PORT_DIPNAME( 0x03, 0x02, DEF_STR( Lives ) )
	PORT_DIPSETTING(    0x03, "2" )
	PORT_DIPSETTING(    0x02, "3" )
	PORT_DIPSETTING(    0x01, "4" )
	PORT_DIPSETTING(    0x00, "7" )
	PORT_DIPNAME( 0x04, 0x04, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x04, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x18, 0x18, DEF_STR( Bonus_Life ) )
	PORT_DIPSETTING(    0x18, "30000 150000" )
	PORT_DIPSETTING(    0x10, "50000 200000" )
	PORT_DIPSETTING(    0x08, "30000" )
	PORT_DIPSETTING(    0x00, "50000" )
	PORT_DIPNAME( 0x60, 0x60, DEF_STR( Difficulty ) )
	PORT_DIPSETTING(    0x60, "Easy" )
	PORT_DIPSETTING(    0x40, "Medium" )
	PORT_DIPSETTING(    0x20, "Hard" )
	PORT_DIPSETTING(    0x00, "Hardest" )
	PORT_DIPNAME( 0x80, 0x00, DEF_STR( Demo_Sounds ) )
	PORT_DIPSETTING(    0x80, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )

	PORT_START	/* player 1 8-way rotary control - converted in rotary_0_r() */
	PORT_ANALOG( 0xff, 0x00, IPT_DIAL | IPF_PLAYER7, 25, 10, 0, 0 )

	PORT_START	/* player 2 8-way rotary control - converted in rotary_1_r() */
	PORT_ANALOG( 0xff, 0x00, IPT_DIAL | IPF_PLAYER8, 25, 10, 0, 0 )
INPUT_PORTS_END



static struct GfxLayout charlayout =
{
	8,8,
	RGN_FRAC(1,4),
	8,	/* 8 bits per pixel (!) */
	{ 0, 1, 2, 3, RGN_FRAC(1,2)+0, RGN_FRAC(1,2)+1, RGN_FRAC(1,2)+2, RGN_FRAC(1,2)+3 },
	{ 0*4, 1*4, 2*4, 3*4, 4*4, 5*4, 6*4, 7*4 },
	{ 0*32, 1*32, 2*32, 3*32, 4*32, 5*32, 6*32, 7*32 },
	32*8
};

static struct GfxLayout spritelayout =
{
	16,16,
	RGN_FRAC(1,4),
	4,
	{ 0, 1, 2, 3 },
	{ 0*4, 1*4, 2*4, 3*4, 4*4, 5*4, 6*4, 7*4,
			32*8+0*4, 32*8+1*4, 32*8+2*4, 32*8+3*4, 32*8+4*4, 32*8+5*4, 32*8+6*4, 32*8+7*4 },
	{ 0*32, 1*32, 2*32, 3*32, 4*32, 5*32, 6*32, 7*32,
			16*32, 17*32, 18*32, 19*32, 20*32, 21*32, 22*32, 23*32 },
	32*32
};

static struct GfxLayout spritelayout8 =
{
	8,8,
	RGN_FRAC(1,4),
	4,
	{ 0, 1, 2, 3 },
	{ 0*4, 1*4, 2*4, 3*4, 4*4, 5*4, 6*4, 7*4 },
	{ 0*32, 1*32, 2*32, 3*32, 4*32, 5*32, 6*32, 7*32 },
	32*8
};

static struct GfxDecodeInfo jackal_gfxdecodeinfo[] =
{
	{ REGION_GFX1, 0x00000, &charlayout,               0, 16 },	/* colors 256-511 with lookup */
	{ REGION_GFX1, 0x20000, &spritelayout,        256*16, 16 },	/* colors   0- 15 with lookup */
	{ REGION_GFX1, 0x20000, &spritelayout8,       256*16, 16 },	/* to handle 8x8 sprites */
	{ REGION_GFX1, 0x60000, &spritelayout,  256*16+16*16, 16 },	/* colors  16- 31 with lookup */
	{ REGION_GFX1, 0x60000, &spritelayout8, 256*16+16*16, 16 },	/* to handle 8x8 sprites */
	{ -1 } /* end of array */
};



static struct YM2151interface ym2151_interface =
{
	1,
	3580000,
	{ YM3012_VOL(50,MIXER_PAN_LEFT,50,MIXER_PAN_RIGHT) },
	{ 0 },
};


static MACHINE_DRIVER_START( jackal )

	/* basic machine hardware */
	MDRV_CPU_ADD(M6809, 2000000)	/* 2 MHz???? */
	MDRV_CPU_MEMORY(jackal_readmem,jackal_writemem)
	MDRV_CPU_VBLANK_INT(jackal_interrupt,1)

	MDRV_CPU_ADD(M6809, 2000000)	/* 2 MHz???? */
	MDRV_CPU_MEMORY(jackal_sound_readmem,jackal_sound_writemem)

	MDRV_FRAMES_PER_SECOND(60)
	MDRV_VBLANK_DURATION(DEFAULT_60HZ_VBLANK_DURATION)
	MDRV_INTERLEAVE(10)	/* 10 CPU slices per frame - seems enough to keep the CPUs in sync */

	MDRV_MACHINE_INIT(jackal)

	/* video hardware */
	MDRV_VIDEO_ATTRIBUTES(VIDEO_TYPE_RASTER)
	MDRV_SCREEN_SIZE(32*8, 32*8)
	MDRV_VISIBLE_AREA(1*8, 31*8-1, 2*8, 30*8-1)
	MDRV_GFXDECODE(jackal_gfxdecodeinfo)
	MDRV_PALETTE_LENGTH(512)
	MDRV_COLORTABLE_LENGTH(256*16+16*16+16*16)

	MDRV_PALETTE_INIT(jackal)
	MDRV_VIDEO_START(jackal)
	MDRV_VIDEO_UPDATE(jackal)

	/* sound hardware */
	MDRV_SOUND_ATTRIBUTES(SOUND_SUPPORTS_STEREO)
	MDRV_SOUND_ADD(YM2151, ym2151_interface)
MACHINE_DRIVER_END



/***************************************************************************

  Game driver(s)

***************************************************************************/

ROM_START( jackal ) /* 8-Way Joystick: You can only shoot in one direction regardless of travel - up the screen */
	ROM_REGION( 0x20000, REGION_CPU1, 0 )	/* Banked 64k for 1st CPU */
	ROM_LOAD( "631_v02.15d", 0x04000, 0x8000, CRC(0b7e0584) SHA1(e4019463345a4c020d5a004c9a400aca4bdae07b) )
	ROM_CONTINUE(            0x14000, 0x8000 )
	ROM_LOAD( "631_v03.16d", 0x0c000, 0x4000, CRC(3e0dfb83) SHA1(5ba7073751eee33180e51143b348256597909516) )


	ROM_REGION( 0x10000, REGION_CPU2, 0 )     /* 64k for 2nd cpu (Graphics & Sound)*/
	ROM_LOAD( "631_t01.11d", 0x8000, 0x8000, CRC(b189af6a) SHA1(f7df996c394fdd6f2ce128a8df38d7838f7ec6d6) )

	ROM_REGION( 0x80000, REGION_GFX1, ROMREGION_DISPOSE )
	ROM_LOAD16_BYTE( "631t04.7h",  0x00000, 0x20000, CRC(457f42f0) SHA1(08413a13d128875dddcf4f6ad302363096bf1d41) ) /* Silkscreened MASK1M */
	ROM_LOAD16_BYTE( "631t05.8h",  0x00001, 0x20000, CRC(732b3fc1) SHA1(7e89650b9e5e2b7ae82f8c55ac9995740f6fdfe1) ) /* Silkscreened MASK1M */
	ROM_LOAD16_BYTE( "631t06.12h", 0x40000, 0x20000, CRC(2d10e56e) SHA1(447b464ea725fb9ef87da067a41bcf463b427cce) ) /* Silkscreened MASK1M */
	ROM_LOAD16_BYTE( "631t07.13h", 0x40001, 0x20000, CRC(4961c397) SHA1(b430df58fc3bb722d6fb23bed7d04afdb7e5d9c1) ) /* Silkscreened MASK1M */

	ROM_REGION( 0x0200, REGION_PROMS, 0 )	/* color lookup tables */
	ROM_LOAD( "631r08.9h",  0x0000, 0x0100, CRC(7553a172) SHA1(eadf1b4157f62c3af4602da764268df954aa0018) ) /* MMI 63S141AN or compatible (silkscreened 6301) */
	ROM_LOAD( "631r09.14h", 0x0100, 0x0100, CRC(a74dd86c) SHA1(571f606f8fc0fd3d98d26761de79ccb4cc9ab044) ) /* MMI 63S141AN or compatible (silkscreened 6301) */
ROM_END

ROM_START( jackalr ) /* Rotary Joystick: Shot direction is controlled via the rotary function of the joystick */
	ROM_REGION( 0x20000, REGION_CPU1, 0 )	/* Banked 64k for 1st CPU */
	ROM_LOAD( "631_q02.15d", 0x04000, 0x8000, CRC(ed2a7d66) SHA1(3d9b31fa8b31e509880d617feb0dd4bd9790d2d5) )
	ROM_CONTINUE(            0x14000, 0x8000 )
	ROM_LOAD( "631_q03.16d", 0x0c000, 0x4000, CRC(b9d34836) SHA1(af23a0c844fb9e60a757511ca898d73eef4c2e51) )

	ROM_REGION( 0x10000, REGION_CPU2, 0 )     /* 64k for 2nd cpu (Graphics & Sound)*/
	ROM_LOAD( "631_q01.11d", 0x8000, 0x8000, CRC(54aa2d29) SHA1(ebc6b3a5db5120cc33d62e3213d0e881f658282d) )

  ROM_REGION( 0x80000, REGION_GFX1, ROMREGION_DISPOSE )
	ROM_LOAD16_BYTE( "631t04.7h",  0x00000, 0x20000, CRC(457f42f0) SHA1(08413a13d128875dddcf4f6ad302363096bf1d41) ) /* Silkscreened MASK1M */
	ROM_LOAD16_BYTE( "631t05.8h",  0x00001, 0x20000, CRC(732b3fc1) SHA1(7e89650b9e5e2b7ae82f8c55ac9995740f6fdfe1) ) /* Silkscreened MASK1M */
	ROM_LOAD16_BYTE( "631t06.12h", 0x40000, 0x20000, CRC(2d10e56e) SHA1(447b464ea725fb9ef87da067a41bcf463b427cce) ) /* Silkscreened MASK1M */
	ROM_LOAD16_BYTE( "631t07.13h", 0x40001, 0x20000, CRC(4961c397) SHA1(b430df58fc3bb722d6fb23bed7d04afdb7e5d9c1) ) /* Silkscreened MASK1M */

	ROM_REGION( 0x0200, REGION_PROMS, 0 )	/* color lookup tables */
	ROM_LOAD( "631r08.9h",  0x0000, 0x0100, CRC(7553a172) SHA1(eadf1b4157f62c3af4602da764268df954aa0018) ) /* MMI 63S141AN or compatible (silkscreened 6301) */
	ROM_LOAD( "631r09.14h", 0x0100, 0x0100, CRC(a74dd86c) SHA1(571f606f8fc0fd3d98d26761de79ccb4cc9ab044) ) /* MMI 63S141AN or compatible (silkscreened 6301) */
ROM_END

ROM_START( topgunr )/* 8-Way Joystick:  You can only shoot in one direction regardless of travel - up the screen */
	ROM_REGION( 0x20000, REGION_CPU1, 0 )	/* Banked 64k for 1st CPU */
	ROM_LOAD( "631_u02.15d", 0x04000, 0x8000, CRC(f7e28426) SHA1(db2d5f252a574b8aa4d8406a8e93b423fd2a7fef) )
	ROM_CONTINUE(            0x14000, 0x8000 )
	ROM_LOAD( "631_u03.16d", 0x0c000, 0x4000, CRC(c086844e) SHA1(4d6f27ac3aabb4b2d673aa619e407e417ad89337) )


	ROM_REGION( 0x10000, REGION_CPU2, 0 )     /* 64k for 2nd cpu (Graphics & Sound)*/
	ROM_LOAD( "631_t01.11d", 0x8000, 0x8000, CRC(b189af6a) SHA1(f7df996c394fdd6f2ce128a8df38d7838f7ec6d6) )

	ROM_REGION( 0x80000, REGION_GFX1, ROMREGION_DISPOSE )
	ROM_LOAD16_BYTE( "631u04.7h",  0x00000, 0x20000, CRC(50122a12) SHA1(c9e0132a3a40d9d28685c867c70231947d8a9cb7) ) /* Silkscreened MASK1M */
	ROM_LOAD16_BYTE( "631u05.8h",  0x00001, 0x20000, CRC(6943b1a4) SHA1(40de2b434600ea4c8fb42e6b21be2c3705a55d67) ) /* Silkscreened MASK1M */
	ROM_LOAD16_BYTE( "631u06.12h", 0x40000, 0x20000, CRC(37dbbdb0) SHA1(f94db780d69e7dd40231a75629af79469d957378) ) /* Silkscreened MASK1M */
	ROM_LOAD16_BYTE( "631u07.13h", 0x40001, 0x20000, CRC(22effcc8) SHA1(4d174b0ce64def32050f87343c4b1424e0fef6f7) ) /* Silkscreened MASK1M */

	ROM_REGION( 0x0200, REGION_PROMS, 0 )	/* color lookup tables */
	ROM_LOAD( "631r08.9h",  0x0000, 0x0100, CRC(7553a172) SHA1(eadf1b4157f62c3af4602da764268df954aa0018) ) /* MMI 63S141AN or compatible (silkscreened 6301) */
	ROM_LOAD( "631r09.14h", 0x0100, 0x0100, CRC(a74dd86c) SHA1(571f606f8fc0fd3d98d26761de79ccb4cc9ab044) ) /* MMI 63S141AN or compatible (silkscreened 6301) */
ROM_END

ROM_START( jackalj ) /* 8-Way Joystick: You can only shoot in the direction you're traveling */
	ROM_REGION( 0x20000, REGION_CPU1, 0 )	/* Banked 64k for 1st CPU */
	ROM_LOAD( "631_t02.15d", 0x04000, 0x8000, CRC(14db6b1a) SHA1(b469ea50aa94a2bda3bd0442300aa1272e5f30c4) )
	ROM_CONTINUE(            0x14000, 0x8000 )
	ROM_LOAD( "631_t03.16d", 0x0c000, 0x4000, CRC(fd5f9624) SHA1(2520c1ff54410ef498ecbf52877f011900baed4c) )

	ROM_REGION( 0x10000, REGION_CPU2, 0 )     /* 64k for 2nd cpu (Graphics & Sound)*/
	ROM_LOAD( "631_t01.11d", 0x8000, 0x8000, CRC(b189af6a) SHA1(f7df996c394fdd6f2ce128a8df38d7838f7ec6d6) )

	ROM_REGION( 0x80000, REGION_GFX1, ROMREGION_DISPOSE )
	ROM_LOAD16_BYTE( "631t04.7h",  0x00000, 0x20000, CRC(457f42f0) SHA1(08413a13d128875dddcf4f6ad302363096bf1d41) ) /* Silkscreened MASK1M */
	ROM_LOAD16_BYTE( "631t05.8h",  0x00001, 0x20000, CRC(732b3fc1) SHA1(7e89650b9e5e2b7ae82f8c55ac9995740f6fdfe1) ) /* Silkscreened MASK1M */
	ROM_LOAD16_BYTE( "631t06.12h", 0x40000, 0x20000, CRC(2d10e56e) SHA1(447b464ea725fb9ef87da067a41bcf463b427cce) ) /* Silkscreened MASK1M */
	ROM_LOAD16_BYTE( "631t07.13h", 0x40001, 0x20000, CRC(4961c397) SHA1(b430df58fc3bb722d6fb23bed7d04afdb7e5d9c1) ) /* Silkscreened MASK1M */

	ROM_REGION( 0x0200, REGION_PROMS, 0 )	/* color lookup tables */
	ROM_LOAD( "631r08.9h",  0x0000, 0x0100, CRC(7553a172) SHA1(eadf1b4157f62c3af4602da764268df954aa0018) ) /* MMI 63S141AN or compatible (silkscreened 6301) */
	ROM_LOAD( "631r09.14h", 0x0100, 0x0100, CRC(a74dd86c) SHA1(571f606f8fc0fd3d98d26761de79ccb4cc9ab044) ) /* MMI 63S141AN or compatible (silkscreened 6301) */
ROM_END

ROM_START( topgunbl )
	ROM_REGION( 0x20000, REGION_CPU1, 0 )	/* Banked 64k for 1st CPU */
	ROM_LOAD( "t-3.c5", 0x04000, 0x8000, CRC(7826ad38) SHA1(875e87867924905b9b83bc203eb7ffe81cf72233) )
	ROM_LOAD( "t-4.c4", 0x14000, 0x8000, CRC(976c8431) SHA1(c199f57c25380d741aec85b0e0bfb6acf383e6a6) ) /* == 2nd half of 631_q02.15d */
	ROM_LOAD( "t-2.c6", 0x0c000, 0x4000, CRC(d53172e5) SHA1(44b7f180c17f9a121a2f06f2d3471920a8989e21) )

	ROM_REGION( 0x10000, REGION_CPU2, 0 )     /* 64k for 2nd cpu (Graphics & Sound)*/
	ROM_LOAD( "t-1.c14", 0x8000, 0x8000, CRC(54aa2d29) SHA1(ebc6b3a5db5120cc33d62e3213d0e881f658282d) ) /* == 631_q01.11d */

	ROM_REGION( 0x80000, REGION_GFX1, ROMREGION_DISPOSE )
	ROM_LOAD16_WORD_SWAP( "t-17.n12", 0x00000, 0x08000, CRC(e8875110) SHA1(73f4c47ab039dce8c285bf222253084c860c95bf) )
	ROM_LOAD16_WORD_SWAP( "t-18.n13", 0x08000, 0x08000, CRC(cf14471d) SHA1(896aa8d7c93f837f6661d30bd0d6e19d16669107) )
	ROM_LOAD16_WORD_SWAP( "t-19.n14", 0x10000, 0x08000, CRC(46ee5dd2) SHA1(1a910984a197af341f13b4683babee857aafb245) )
	ROM_LOAD16_WORD_SWAP( "t-20.n15", 0x18000, 0x08000, CRC(3f472344) SHA1(49b9da8741b8e474d25726a706cf3008096ab2dc) )
	ROM_LOAD16_WORD_SWAP( "t-6.n1",   0x20000, 0x08000, CRC(539cc48c) SHA1(476ff5fe239e5acb61ede4d745d327f6bc3709f3) )
	ROM_LOAD16_WORD_SWAP( "t-5.m1",   0x28000, 0x08000, CRC(dbc26afe) SHA1(faab1feae91a9c22c008555955596c55d77b70c7) )
	ROM_LOAD16_WORD_SWAP( "t-7.n2",   0x30000, 0x08000, CRC(0ecd31b1) SHA1(06d77159ed55c1e288f2a194cdb09d29542e06d6) )
	ROM_LOAD16_WORD_SWAP( "t-8.n3",   0x38000, 0x08000, CRC(f946ada7) SHA1(fd9a0786436cbdb4c844f71342232e4e6645d98f) )
	ROM_LOAD16_WORD_SWAP( "t-13.n8",  0x40000, 0x08000, CRC(5d669abb) SHA1(faba6d7b47caae2ecdf15fb3527824bdb22e3d6b) )
	ROM_LOAD16_WORD_SWAP( "t-14.n9",  0x48000, 0x08000, CRC(f349369b) SHA1(f19238ef5feb1c89ef58c17a2506cc96ed8054e1) )
	ROM_LOAD16_WORD_SWAP( "t-15.n10", 0x50000, 0x08000, CRC(7c5a91dd) SHA1(85a1d76efc385e8e971a65e225de7f5d100bfbc7) )
	ROM_LOAD16_WORD_SWAP( "t-16.n11", 0x58000, 0x08000, CRC(5ec46d8e) SHA1(350e983b56a9f7d95e98429ee9a5fa6d3af36db4) )
	ROM_LOAD16_WORD_SWAP( "t-9.n4",   0x60000, 0x08000, CRC(8269caca) SHA1(8b80b7bad966d5b61a5c22d2ced625b5645f2ce2) )
	ROM_LOAD16_WORD_SWAP( "t-10.n5",  0x68000, 0x08000, CRC(25393e4f) SHA1(f6d7995b51d5bbbc3e325d6949dbc435446b5cf9) )
	ROM_LOAD16_WORD_SWAP( "t-11.n6",  0x70000, 0x08000, CRC(7895c22d) SHA1(c81ae51116fb32ac99d37eb7c2000c990d089b8d) )
	ROM_LOAD16_WORD_SWAP( "t-12.n7",  0x78000, 0x08000, CRC(15606dfc) SHA1(829492da49dbe70f81d15237803c5203aa011957) )

	ROM_REGION( 0x0200, REGION_PROMS, 0 )	/* color lookup tables */
	ROM_LOAD( "631r08.bpr",   0x0000, 0x0100, CRC(7553a172) SHA1(eadf1b4157f62c3af4602da764268df954aa0018) )
	ROM_LOAD( "631r09.bpr",   0x0100, 0x0100, CRC(a74dd86c) SHA1(571f606f8fc0fd3d98d26761de79ccb4cc9ab044) )
ROM_END



GAMEX( 1986, jackal,   0,      jackal, jackal,   0, ROT90, "Konami", "Jackal (World, 8-way Joystick)", GAME_IMPERFECT_COLORS | GAME_NO_COCKTAIL )
GAMEX( 1986, jackalr,  jackal, jackal, topgunbl, 0, ROT90, "Konami", "Jackal (World, Rotary Joystick)", GAME_IMPERFECT_COLORS | GAME_NO_COCKTAIL )
GAMEX( 1986, topgunr,  jackal, jackal, jackal,   0, ROT90, "Konami", "Top Gunner (US, 8-way Joystick)", GAME_IMPERFECT_COLORS | GAME_NO_COCKTAIL )
GAMEX( 1986, jackalj,  jackal, jackal, jackal,   0, ROT90, "Konami", "Tokushu Butai Jackal (Japan, 8-way Joystick)", GAME_IMPERFECT_COLORS | GAME_NO_COCKTAIL )
GAMEX( 1986, topgunbl, jackal, jackal, topgunbl, 0, ROT90, "bootleg", "Top Gunner (bootleg)", GAME_IMPERFECT_COLORS | GAME_NO_COCKTAIL )
