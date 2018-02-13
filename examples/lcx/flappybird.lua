local ej = require "ejoy2d"
local fw = require "ejoy2d.framework"
local pack = require "ejoy2d.simplepackage"
local matrix = require "ejoy2d.matrix"

local config = require "examples.asset.bird_config"

pack.load {
	pattern = fw.WorkDir..[[examples/asset/?]],
	"birds",
}

local screen_width = 1024
local screen_height = 768
local PI2 = math.pi * 2
local sky1 = ej.sprite("birds", "sky_bg")
local sky2 = ej.sprite("birds", "sky_bg")
local land1 = ej.sprite("birds", "land_bg")
local land2 = ej.sprite("birds", "land_bg")

local scoreboard = ej.sprite("birds", "scoreboard")
scoreboard.text = "0"

local function _width(s, scale)
  return -s:aabb({x=0,y=0, scale=scale})
end
local function _half_height(s, scale)
  local _, h1, _, h2 = s:aabb({x=0,y=0, scale=scale})
  return (-h1 + h2)/2
end
--------------------------------------------------------------------------------
local bg_speed = -100

local Cbg = {}
function Cbg.new( sprite1, sprite2, posy, speed )
	local sprite_width = _width( sprite1 )
	local instance = {
		sprite1 = sprite1, sprite2 = sprite2, x1 = 0, y1 = posy,
		x2 = 0 + sprite_width, y2 = posy,
		speed = speed,
		sprite_width = sprite_width,
	}
	setmetatable( instance, {
		__index = Cbg,
	} )
	return instance
end 

function Cbg:draw()
	self.sprite1:draw( {x=self.x1,y=self.y1} )
	self.sprite2:draw( {x=self.x2,y=self.y2} )
end 

function Cbg:__rollBg( dt )
	local movex = self.speed * dt
	self.x1 = self.x1 + movex
	self.x2 = self.x2 + movex
	if self.x1 < -self.sprite_width then
		self.x1 = self.x2 + self.sprite_width

	elseif self.x2 < -self.sprite_width then 
		self.x2 = self.x1 + self.sprite_width
	end 
end

function Cbg:update( dt )
	self:__rollBg( dt )
end 

local bgSky = Cbg.new( sky1, sky2, screen_height/4, bg_speed )
local bgLand = Cbg.new( land1, land2,  screen_height-_half_height(land1), bg_speed )

--pipes
local Cpipe = {}
function Cpipe.new()
	local sprite = ej.sprite( "birds", "..." )
	local instance = {
		sprite = sprite
	}
	setmetatable( instance, {
		__index = Cpipe,
	} )
end 



-----------------------------------------------------------------------------------
local birdSprite = ej.sprite( "birds", "bird" )
local bird = {
	sprite = birdSprite,
	x = 300,
	y = screen_height/2,
	speed = 0,
	a = 0,
}
local gravity = 700
local touch_speed_change = -280.5

function bird:draw()
	self.sprite:draw( {x=self.x, y=self.y}  )
end 

function bird:__changeA( offa )
	self.a = self.a + offa
end 

function bird:__changeSpeed( offv )
	self.speed = self.speed + offv
end 

function bird:__changePos( dx, dy )
	self.x = self.x + dx; self.y = self.y + dy
end

function bird:gotTouch()
	self.speed = touch_speed_change
	self.a = 0
end 

function bird:__updateRotation()
  local speedx = math.abs( bg_speed )
  local speedy = math.abs( self.speed )

  local degree = math.atan( speedy, speedx ) * PI2
  degree = degree * 2
  if degree > 90 then degree = 90 end
  -- print( "speedx:", speedx )
  -- print( "speedy:", speedy )
  -- print( "degree:", degree )
  if self.speed > 0 then
    self.sprite:sr( degree )
  else
    self.sprite:sr( -degree )
  end

end

function bird:update( dt )
	self:__changeA( gravity*dt )
	self:__changeSpeed( self.a * dt )
	self:__changePos( 0, self.speed * dt )
	self:__updateRotation()
end 


---------------------------------------------------------------------------------


local game = {}
local g_LastUpdate = os.clock()
function game.update(  )
	local timeNow = os.clock()
	local dt = timeNow - g_LastUpdate
	g_LastUpdate = timeNow

	-- print( "update:", dt )
	bgSky:update( dt )
	bgLand:update( dt )
	bird:update( dt )
end

function game.drawframe()
	ej.clear(0xff51c0c9)	-- clear blue

	bgSky:draw()
	bgLand:draw()
	bird:draw()
end

function game.touch(what, x, y)
  if what == "BEGIN" then
    bird:gotTouch()
  end
end

function game.message(...)
end

function game.handle_error(...)
end

function game.on_resume()
end

function game.on_pause()
end

ej.start(game)



