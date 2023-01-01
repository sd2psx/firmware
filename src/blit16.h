/* ISC License
 * Copyright (c) 2018 Andrew Reece
 *
 * Permission to use, copy, modify, and/or distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 * OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */
#ifndef blit16_H

#define blit16_WIDTH  3
#define blit16_HEIGHT 5
#define blit16_ADVANCE (blit16_WIDTH + 1)
#define blit16_DESCENDER 1
#define blit16_BASELINE_OFFSET (blit16_DESCENDER + 1)
#define blit16_ROW_ADVANCE (blit16_HEIGHT + blit16_BASELINE_OFFSET)
#define blit16_EXTRA_BITS(x) (((x) >> 15) & 1)

typedef unsigned short blit16_glyph;

#ifndef blit_H
#ifndef blit_pixel
#define blit_pixel unsigned int
#endif/*blit_pixel*/
#ifndef blit_NO_INLINE
#define blit_inline inline
#else /*blit_NO_INLINE*/
#define blit_inline
#endif/*blit_NO_INLINE*/
#define blit_NUM_GLYPHS 95
#define blit_IndexFromASCII(ascii) (ascii - ' ')
#define blit_ASCIIFromIndex(index) (index + ' ')
typedef struct blit_props
{
	blit_pixel *Buffer;
	blit_pixel  Value;
	       int  Scale;
	       int  BufWidth;
	       int  BufHeight;
	enum {blit_Clip, blit_Wrap} Wrap;
} blit_props;
#define blit_H
#endif/*blit_H*/

#ifndef blit16_ARRAY_ONLY
#define blit16_Glyphs Blit16.Glyphs
typedef struct blit16_font
{
	const blit16_glyph Glyphs[blit_NUM_GLYPHS];
	const unsigned int Width;
	const unsigned int Height;
	const unsigned int Descender;
	const unsigned int Advance;
	const unsigned int RowAdvance;
	        blit_props Props;
} blit16_font;

#define blit16_SCALE(Font, scale) Font->Props.Scale = scale;         \
	*(unsigned int *)&Font->Width      = blit16_WIDTH       * scale; \
	*(unsigned int *)&Font->Height     = blit16_HEIGHT      * scale; \
	*(unsigned int *)&Font->Descender  = blit16_DESCENDER   * scale; \
	*(unsigned int *)&Font->Advance    = blit16_ADVANCE     * scale; \
	*(unsigned int *)&Font->RowAdvance = blit16_ROW_ADVANCE * scale
#ifndef blit16_MACRO_INLINE
blit_inline void blit16_Scale(blit16_font *Font, int Scale) { blit16_SCALE(Font, Scale); }
#else /*blit16_MACRO_INLINE*/
#define blit16_Scale(Font, scale) \
do { int blit16_TempScale = (scale); blit16_SCALE((Font), blit16_TempScale); } while (0)
#endif/*blit16_MACRO_INLINE*/

blit16_font Blit16 = {
#else/*blit16_ARRAY_ONLY*/
blit16_glyph blit16_Glyphs[blit_NUM_GLYPHS] =
#endif/*blit16_ARRAY_ONLY*/
	{
		/* all chars up to 32 are non-printable */
		0x0000,0x2092,0x002d,0x5f7d,0x279e,0x52a5,0x7ad6,0x0012,
		0x4494,0x1491,0x0aba,0x05d0,0x1400,0x01c0,0x0400,0x12a4,
		0x2b6a,0x749a,0x752a,0x38a3,0x4f4a,0x38cf,0x3bce,0x12a7,
		0x3aae,0x49ae,0x0410,0x1410,0x4454,0x0e38,0x1511,0x10e3,
		0x73ee,0x5f7a,0x3beb,0x624e,0x3b6b,0x73cf,0x13cf,0x6b4e,
		0x5bed,0x7497,0x2b27,0x5add,0x7249,0x5b7d,0x5b6b,0x3b6e,
		0x12eb,0x4f6b,0x5aeb,0x388e,0x2497,0x6b6d,0x256d,0x5f6d,
		0x5aad,0x24ad,0x72a7,0x6496,0x4889,0x3493,0x002a,0xf000,
		0x0011,0x6b98,0x3b79,0x7270,0x7b74,0x6750,0x95d6,0xb9ee,
		0x5b59,0x6410,0xb482,0x56e8,0x6492,0x5be8,0x5b58,0x3b70,
		0x976a,0xcd6a,0x1370,0x38f0,0x64ba,0x3b68,0x2568,0x5f68,
		0x54a8,0xb9ad,0x73b8,0x64d6,0x2492,0x3593,0x03e0,
#ifndef blit16_ARRAY_ONLY
	},
	blit16_WIDTH,
	blit16_HEIGHT,
	blit16_DESCENDER,
	blit16_ADVANCE,
	blit16_ROW_ADVANCE,
#endif/*blit16_ARRAY_ONLY*/
};

/* StartX/Y refers to the top left corner of the glyph's bounding box */
int blit16_TextNExplicit(blit_pixel *Buffer, blit_pixel Value, int Scale, int BufWidth, int BufHeight, int Wrap, int StartX, int StartY, int StrLen, char *String)
{
	int IsNegative = BufWidth < 0;
	int DrawDir = IsNegative ? -1 : 1;
	int AbsBufWidth = DrawDir * BufWidth;
	unsigned int LinesPrinted = 1;
	int i, x, y;
	StrLen = StrLen >= 0 ? StrLen : 0x7FFFFFFF; /* if negative, wrap to max int */
	for(i = 0, x = StartX, y = StartY; String[i] && i < StrLen; ++i)
	{
		char c = String[i];
		int EndY = y + (blit16_HEIGHT + blit16_DESCENDER) * Scale * DrawDir;
		int BufUnderflow  = EndY < 0 || y < 0;
		int BufOverflow   = BufHeight && (EndY >= BufHeight || y >= BufHeight);
		int BufYMinExceed = BufUnderflow || (IsNegative && BufOverflow);
		int BufYMaxExceed = BufOverflow  || (IsNegative && BufUnderflow);
		int BufXMinExceed = x < 0;
		int EndX = c == '\n' || c == '\r' ? StartX            :
		               c == '\t' ? x + 4 * Scale * blit16_ADVANCE :
		               c == '\b' ?       x - Scale * blit16_WIDTH :
		               /* normal char */ x + Scale * blit16_WIDTH;
		int BufXMaxExceed = EndX > AbsBufWidth;

		if(BufYMaxExceed) { break; } /* no point adding extra undrawable lines */
		else if((! BufYMinExceed && (! BufXMaxExceed || Wrap)) || c == '\n')
		{
			if(BufXMaxExceed && c != '\n') { c = '\n'; --i; } /* new line and redo on-screen checks */
			else if(BufXMinExceed)         { c = ' '; }       /* skip past character without drawing */
			switch(c)
			{
				default:                                                 /* normal character */
				{
					unsigned int glY, pxY, glX, pxX;
					blit16_glyph Glyph = blit16_Glyphs[blit_IndexFromASCII(c)];
					unsigned int OffsetY = y + blit16_EXTRA_BITS(Glyph) * Scale * DrawDir;
					blit_pixel *Pixel, *Row = Buffer + OffsetY * AbsBufWidth + x;
					for(glY = 0; glY < blit16_HEIGHT; ++glY)
						for(pxY = Scale; pxY--; Row += BufWidth)
							for(glX = 0, Pixel = Row; glX < blit16_WIDTH; ++glX)
							{
								unsigned int Shift = glY * blit16_WIDTH + glX;
								unsigned int PixelDrawn = (Glyph >> Shift) & 1;
								if(PixelDrawn) for(pxX = Scale; pxX--; *Pixel++ = Value);
								else { Pixel += Scale; }
							}
				} /* fallthrough */
				case  ' ': x +=           Scale * blit16_ADVANCE; break; /* space: no need to touch pixels */
				case '\b': x -=           Scale * blit16_ADVANCE; break; /* non-destructive backspace */
				case '\t': x +=       4 * Scale * blit16_ADVANCE; break; /* tab: add 4 spaces */
				case '\n': y += DrawDir * Scale * blit16_ROW_ADVANCE; ++LinesPrinted; /* new line; fallthrough */
				case '\r': x  = StartX;                        break; /* carriage return */
			}
		}
	}
	return LinesPrinted;
}

#ifndef blit16_NO_HELPERS
#ifndef blit16_MACRO_INLINE

blit_inline int blit16_TextExplicit(blit_pixel *Buffer, blit_pixel Value, int Scale, int BufWidth, int BufHeight, int Wrap, int StartX, int StartY, char *String)
{ return blit16_TextNExplicit(Buffer, Value, Scale, BufWidth, BufHeight, Wrap, StartX, StartY, -1, String); }
blit_inline int blit16_TextNProps(blit_props Props, int StartX, int StartY, int StrLen, char *String)
{ return blit16_TextNExplicit(Props.Buffer, Props.Value, Props.Scale, Props.BufWidth, Props.BufHeight, Props.Wrap, StartX, StartY, StrLen, String); }
blit_inline int blit16_TextProps(blit_props Props, int StartX, int StartY, char *String)
{ return blit16_TextNExplicit(Props.Buffer, Props.Value, Props.Scale, Props.BufWidth, Props.BufHeight, Props.Wrap, StartX, StartY, -1, String); }
blit_inline int blit16_TextN(int StartX, int StartY, int StrLen, char *String)
{ return blit16_TextNExplicit(Blit16.Props.Buffer, Blit16.Props.Value, Blit16.Props.Scale, Blit16.Props.BufWidth, Blit16.Props.BufHeight, Blit16.Props.Wrap, StartX, StartY, StrLen, String); }
blit_inline int blit16_Text(int StartX, int StartY, char *String)
{ return blit16_TextNExplicit(Blit16.Props.Buffer, Blit16.Props.Value, Blit16.Props.Scale, Blit16.Props.BufWidth, Blit16.Props.BufHeight, Blit16.Props.Wrap, StartX, StartY, -1, String); }

#else/*blit16_NO_INLINE*/

#define blit16_TextExplicit(Buffer, Value, Scale, BufWidth, BufHeight, Wrap, StartX, StartY, String) \
	blit16_TextNExplicit(Buffer, Value, Scale, BufWidth, BufHeight, Wrap, StartX, StartY, -1, String)
#define blit16_TextNProps(Props, StartX, StartY, StrLen, String) \
	blit16_TextNExplicit((Props).Buffer, (Props).Value, (Props).Scale, (Props).BufWidth, (Props).BufHeight, (Props).Wrap, StartX, StartY, StrLen, String)
#define blit16_TextProps(Props, StartX, StartY, String) \
	blit16_TextNExplicit((Props).Buffer, (Props).Value, (Props).Scale, (Props).BufWidth, (Props).BufHeight, (Props).Wrap, StartX, StartY, -1, String)
#define blit16_TextN(StartX, StartY, StrLen, String) \
	blit16_TextNExplicit(Blit16.Props.Buffer, Blit16.Props.Value, Blit16.Props.Scale, Blit16.Props.BufWidth, Blit16.Props.BufHeight, Blit16.Props.Wrap, StartX, StartY, StrLen, String)
#define blit16_Text(StartX, StartY, String) \
	blit16_TextNExplicit(Blit16.Props.Buffer, Blit16.Props.Value, Blit16.Props.Scale, Blit16.Props.BufWidth, Blit16.Props.BufHeight, Blit16.Props.Wrap, StartX, StartY, -1, String)

#endif/*blit16_NO_INLINE*/
#endif/*blit16_NO_HELPERS*/

#define blit16_H
#endif/*blit16_H*/
