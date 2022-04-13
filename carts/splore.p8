pico-8 cartridge // http://www.pico-8.com
version 29
__lua__
-- splore v0.1
-- @egordorichev

cartdata("__splore")
local current=max(1,dget(0) or 1)

function read_card()
 title,author=__read_cdata(carts[current][1])
 carts[current]={
  carts[current][1],
  author,
  title
 }
end

function _init()
 carts=__list_carts()
 current=min(#carts,current)
 
 for c in all(carts) do
  title,author,label=__read_cdata(c[1])
  c[2]=author
  c[3]=title=="unknown" and c[1] or title
  c[4]=label
 end

 if #carts == 0 then
  while true do
    cls()
    print("no carts found", 1, 1)
    flip()
  end
 end

 update_bg()
end

function update_bg()
 dset(0,current)
 local c = carts[current]

 if not c then return end

 local d=c[4]
 for i=0,128*128-1 do
  sset(
   i%128,flr(i/128),
   tonum("0x"..
   sub(d,i+1,i+1))
  )
 end
end

function _update()
 if(btnp(⬆️)) current=max(1,current-1) update_bg() sfx(0)
 if(btnp(⬇️)) current=min(#carts,current+1) update_bg() sfx(1)
 if(btnp(❎) or btnp(🅾️) or btnp(6)) then
  __load(carts[current][1]) 
 end
end

function _draw()
 cls()
 sspr(0,0,128,128,0,0)
 
 local x,y=16,72
 local ex,ey=112,127
 
 rectfill(x-1,y-1,ex+1,ey+1,7)
 rectfill(x,y,ex,ey,0)
 clip(x,y,ex-x-1,ey-y+1)
 
 local mod=current>6 and (current-6) or 0
 for i=1,9 do
  local cart=carts[i+mod]
  
  if cart then
   local yy=y+1+(i-1)*8
   local l=#cart[3]*4
   local c=7
   
   if i+mod==current then
    rectfill(x+1,yy,x+2+l,yy+6,14)
    rectfill(x+2+l,yy,ex-1,yy+6,15)
    c=0
   end
   
   print(cart[3],x+2,yy+1,c)
   --print(cart[2],ex-#cart[2]*4,yy+1,c)
   --print((i+mod==current and " ➡️ " or "   ")..cart.name.." @"..cart.author,2,i*7+72)
  end
 end
 clip()
end
__sfx__
010c00000c15500105001050010500105001050010500105001050010500105001050010500105001050010500105001050010500105001050010500105001050010500105001050010500105001050010500105
011000001015500000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000
01100000101540c155000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000
