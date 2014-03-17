//-----------------------------------------------------------------------------------

function MD5Hex(x)
{
var k,d,s,i
  d = "0123456789abcdef";
  s = "";
  for( k=0; k<32; k+=4 )
   {
     i = ((x<<k)>>28)&15
     s += d.substring( i, i+1 )
   }
  return s
}

//-----------------------------------------------------------------------------------

function MD5NBlocks(len)
{
  l = len*8
  pad = 512 - ( (l+64) % 512 )
  return (l+64+pad) / 512
}

//-----------------------------------------------------------------------------------

function MD5GetBlock( s, blk )
{
var k
  buf = Array(64)

  nsym = Math.max( 0, s.length-64*blk )
  nb = MD5NBlocks(s.length)

//  for( k=0; k<nsym; k++ )
//    document.write( MD5Hex(s.charCodeAt(64*blk+k)%848)+"<br>" )

  for( k=0; k<nsym; k++ )
//    buf[k] = s.charCodeAt(64*blk+k)
    buf[k] = s.charCodeAt(64*blk+k)%848

  if( k < 64 )  // block not full
    if( s.length-64*blk >= 0 )  // 0x80 not in prev block
      buf[k++] = 0x80;

  for( ; k<64; k++ )
    buf[k] = 0;

  MD5block = Array(16)
  for( k=0; k<16; k++ )
   {
     a = buf[4*k+0]
     b = buf[4*k+1]
     c = buf[4*k+2]
     d = buf[4*k+3]
     MD5block[k] = (d<<24) | (c<<16) | (b<<8) | a
   }

  if( blk == nb-1 )
    MD5block[14] = s.length*8

  return MD5block
}

//-----------------------------------------------------------------------------------

function MD5F(x,y,z) { return  x&y | (~x)&z ; }
function MD5G(x,y,z) { return  x&z | y&~z   ; }
function MD5H(x,y,z) { return  x ^ y ^ z    ; }
function MD5I(x,y,z) { return  y ^ (x | ~z) ; }

//-----------------------------------------------------------------------------------

function MD5Rot(x,s)
{
  bmask = ((1<<s)-1)&0xffffffff
  amask = (~bmask)&0xffffffff
  a = ( x<<s ) & amask
  b = ( x>>(32-s) ) & bmask
  return a|b
}

//-----------------------------------------------------------------------------------

function MD5FF( pos,  k, s, t )
{  
  MD5Reg[(-pos+0)&3] += MD5F( MD5Reg[(-pos+1)&3], MD5Reg[(-pos+2)&3], MD5Reg[(-pos+3)&3] ) + MD5block[k] + t
  MD5Reg[(-pos+0)&3]  = MD5Rot( MD5Reg[(-pos+0)&3], s)  
  MD5Reg[(-pos+0)&3] += MD5Reg[(-pos+1)&3]
}

//------------------------------

function MD5GG( pos,  k, s, t )
{  
//alert( MD5Hex( MD5G( MD5Reg[(-pos+1)&3], MD5Reg[(-pos+2)&3], MD5Reg[(-pos+3)&3] )  ))
  MD5Reg[(-pos+0)&3] += MD5G( MD5Reg[(-pos+1)&3], MD5Reg[(-pos+2)&3], MD5Reg[(-pos+3)&3] ) + MD5block[k] + t
  MD5Reg[(-pos+0)&3]  = MD5Rot( MD5Reg[(-pos+0)&3], s)  
  MD5Reg[(-pos+0)&3] += MD5Reg[(-pos+1)&3]
}

//------------------------------

function MD5HH( pos,  k, s, t )
{  
  MD5Reg[(-pos+0)&3] += MD5H( MD5Reg[(-pos+1)&3], MD5Reg[(-pos+2)&3], MD5Reg[(-pos+3)&3] ) + MD5block[k] + t
  MD5Reg[(-pos+0)&3]  = MD5Rot( MD5Reg[(-pos+0)&3], s)  
  MD5Reg[(-pos+0)&3] += MD5Reg[(-pos+1)&3]
}

//------------------------------

function MD5II( pos,  k, s, t )
{  
  MD5Reg[(-pos+0)&3] += MD5I( MD5Reg[(-pos+1)&3], MD5Reg[(-pos+2)&3], MD5Reg[(-pos+3)&3] ) + MD5block[k] + t
  MD5Reg[(-pos+0)&3]  = MD5Rot( MD5Reg[(-pos+0)&3], s)  
  MD5Reg[(-pos+0)&3] += MD5Reg[(-pos+1)&3]
}

//-----------------------------------------------------------------------------------

function OutRegs()
{
  document.write( "<pre>" )
  document.write( MD5Hex(MD5Reg[0])+" " )
  document.write( MD5Hex(MD5Reg[1])+" " )
  document.write( MD5Hex(MD5Reg[2])+" " )
  document.write( MD5Hex(MD5Reg[3])+" " )
  document.writeln( "</pre>" )
}

//-----------------------------------------------------------------------------------

function MD5RegsToString()
{
var s,k,i,d
  s = ""
  for( k=0; k<4; k++ )
   {
     d=0
     for( i=0; i<32; i+=8 )
       d = (d<<8) | ( (MD5Reg[k]>>i)&0xFF  )
     s += MD5Hex(d)
   }
  return s
}

//-----------------------------------------------------------------------------------

function MD5( s )
{
  nb = MD5NBlocks( s.length )

  MD5Reg = Array( 0x67452301, 0xefcdab89, 0x98badcfe, 0x10325476 )

  for( k=0; k<nb; k++ )
   {
     MD5block = MD5GetBlock(s,k)

     MD5Reg0 = Array( 4 )
     MD5Reg0[0] = MD5Reg[0]; 
     MD5Reg0[1] = MD5Reg[1]; 
     MD5Reg0[2] = MD5Reg[2]; 
     MD5Reg0[3] = MD5Reg[3];

     MD5FF( 0,  0, 7, 0xd76aa478 )
     MD5FF( 1,  1,12, 0xe8c7b756 )
     MD5FF( 2,  2,17, 0x242070db )
     MD5FF( 3,  3,22, 0xc1bdceee )
     MD5FF( 0,  4, 7, 0xf57c0faf )
     MD5FF( 1,  5,12, 0x4787c62a )
     MD5FF( 2,  6,17, 0xa8304613 )
     MD5FF( 3,  7,22, 0xfd469501 )
     MD5FF( 0,  8, 7, 0x698098d8 )
     MD5FF( 1,  9,12, 0x8b44f7af )
     MD5FF( 2, 10,17, 0xffff5bb1 )
     MD5FF( 3, 11,22, 0x895cd7be )
     MD5FF( 0, 12, 7, 0x6b901122 )
     MD5FF( 1, 13,12, 0xfd987193 )
     MD5FF( 2, 14,17, 0xa679438e )
     MD5FF( 3, 15,22, 0x49b40821 )
     MD5GG( 0,  1, 5, 0xf61e2562 )

     MD5GG( 1,  6, 9, 0xc040b340 )
     MD5GG( 2, 11,14, 0x265e5a51 )
     MD5GG( 3,  0,20, 0xe9b6c7aa )
     MD5GG( 0,  5, 5, 0xd62f105d )
     MD5GG( 1, 10, 9, 0x02441453 )
     MD5GG( 2, 15,14, 0xd8a1e681 )
     MD5GG( 3,  4,20, 0xe7d3fbc8 )
     MD5GG( 0,  9, 5, 0x21e1cde6 )
     MD5GG( 1, 14, 9, 0xc33707d6 )
     MD5GG( 2,  3,14, 0xf4d50d87 )
     MD5GG( 3,  8,20, 0x455a14ed )
     MD5GG( 0, 13, 5, 0xa9e3e905 )
     MD5GG( 1,  2, 9, 0xfcefa3f8 )
     MD5GG( 2,  7,14, 0x676f02d9 )
     MD5GG( 3, 12,20, 0x8d2a4c8a )

     MD5HH( 0,  5, 4, 0xfffa3942 )
     MD5HH( 1,  8,11, 0x8771f681 )
     MD5HH( 2, 11,16, 0x6d9d6122 )
     MD5HH( 3, 14,23, 0xfde5380c )
     MD5HH( 0,  1, 4, 0xa4beea44 )
     MD5HH( 1,  4,11, 0x4bdecfa9 )
     MD5HH( 2,  7,16, 0xf6bb4b60 )
     MD5HH( 3, 10,23, 0xbebfbc70 )
     MD5HH( 0, 13, 4, 0x289b7ec6 )
     MD5HH( 1,  0,11, 0xeaa127fa )
     MD5HH( 2,  3,16, 0xd4ef3085 )
     MD5HH( 3,  6,23, 0x04881d05 )
     MD5HH( 0,  9, 4, 0xd9d4d039 )
     MD5HH( 1, 12,11, 0xe6db99e5 )
     MD5HH( 2, 15,16, 0x1fa27cf8 )
     MD5HH( 3,  2,23, 0xc4ac5665 )

     MD5II( 0,  0, 6, 0xf4292244 )
     MD5II( 1,  7,10, 0x432aff97 )
     MD5II( 2, 14,15, 0xab9423a7 )
     MD5II( 3,  5,21, 0xfc93a039 )
     MD5II( 0, 12, 6, 0x655b59c3 )
     MD5II( 1,  3,10, 0x8f0ccc92 )
     MD5II( 2, 10,15, 0xffeff47d )
     MD5II( 3,  1,21, 0x85845dd1 )
     MD5II( 0,  8, 6, 0x6fa87e4f )
     MD5II( 1, 15,10, 0xfe2ce6e0 )
     MD5II( 2,  6,15, 0xa3014314 )
     MD5II( 3, 13,21, 0x4e0811a1 )
     MD5II( 0,  4, 6, 0xf7537e82 )
     MD5II( 1, 11,10, 0xbd3af235 )
     MD5II( 2,  2,15, 0x2ad7d2bb )
     MD5II( 3,  9,21, 0xeb86d391 )

     MD5Reg[0] += MD5Reg0[0]
     MD5Reg[1] += MD5Reg0[1]
     MD5Reg[2] += MD5Reg0[2]
     MD5Reg[3] += MD5Reg0[3]
   }

  return MD5RegsToString()
}

//-----------------------------------------------------------------------------------
/*
function W(s) { document.write(s+"<br>")}

function Test(s)
{
  W( "MD5 (\""+s+"\") = " + MD5(s) )
}

Test("")
Test("a")
Test("abc")
Test("message digest")
Test("abcdefghijklmnopqrstuvwxyz")
Test("ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789")
Test("12345678901234567890123456789012345678901234567890123456789012345678901234567890")
*/
//-----------------------------------------------------------------------------------

