/*
 * Copyright 2017-2018 Agouti Games Team (see the AUTHORS file)
 *
 * This file is part of the RawSalmonEngine.
 *
 * The RawSalmonEngine is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * The RawSalmonEngine is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with the RawSalmonEngine.  If not, see <http://www.gnu.org/licenses/>.
 */
#include "graphics/texture.hpp"

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include <iostream>

Texture::Texture()
{
	//Initialize
	mTexture = nullptr;
	mWidth = 0;
	mHeight = 0;
}

Texture::~Texture()
{
	//Deallocate
	free();
}

/**
 * @brief Loads the supplied image file to a SDL2 texture
 * @param renderer Supplied renderer to use
 * @param path Path to the image file
 * @return @c bool which indicates success or failure
 */
bool Texture::loadFromFile( SDL_Renderer* renderer, std::string path )
{
	//Get rid of preexisting texture
	free();

	//The final texture
	SDL_Texture* newTexture = nullptr;

	//Load image at specified path
	SDL_Surface* loadedSurface = IMG_Load( path.c_str() );
	if( loadedSurface == nullptr )
	{
		std::cerr << "Unable to load image " << path.c_str() << "! SDL_image Error: " << IMG_GetError() << "\n";
	}
	else
	{
		//Color key image
		//SDL_SetColorKey( loadedSurface, SDL_TRUE, SDL_MapRGB( loadedSurface->format, 0, 0xFF, 0xFF ) );

		//Create texture from surface pixels
        newTexture = SDL_CreateTextureFromSurface( renderer, loadedSurface );
		if( newTexture == nullptr )
		{
			std::cerr << "Unable to create texture from " << path.c_str() << "! SDL_image Error: " << SDL_GetError() << "\n";
		}
		else
		{
			//Get image dimensions
			mWidth = loadedSurface->w;
			mHeight = loadedSurface->h;
		}

		//Get rid of old loaded surface
		SDL_FreeSurface( loadedSurface );
	}

	//Return success
	mTexture = newTexture;
	return mTexture != nullptr;
}
/**
 * @brief Creates a texture from a text
 * @param renderer Supplied renderer to use
 * @param textureText Text to be rendered
 * @param textColor Color of the text
 * @param font The font which is used
 * @return @c bool which indicates success or failure
 */
bool Texture::loadFromRenderedText( SDL_Renderer* renderer, std::string textureText, SDL_Color textColor, TTF_Font* font )
{
	//Get rid of preexisting texture
	free();

	//Render text surface
	SDL_Surface* textSurface = TTF_RenderText_Solid( font, textureText.c_str(), textColor );
	if( textSurface != nullptr )
	{
		//Create texture from surface pixels
        mTexture = SDL_CreateTextureFromSurface( renderer, textSurface );
		if( mTexture == nullptr )
		{
			std::cerr << "Unable to create texture from rendered text! SDL Error: " << SDL_GetError() << "\n";
		}
		else
		{
			//Get image dimensions
			mWidth = textSurface->w;
			mHeight = textSurface->h;
		}

		//Get rid of old surface
		SDL_FreeSurface( textSurface );
	}
	else
	{
		std::cerr << "Unable to render text surface! SDL_ttf Error: " << TTF_GetError() << "\n";
	}


	//Return success
	return mTexture != nullptr;
}

/// Cleans up the hardware texture
void Texture::free()
{
	//Free texture if it exists
	if( mTexture != nullptr )
	{
		SDL_DestroyTexture( mTexture );
		mTexture = nullptr;
		mWidth = 0;
		mHeight = 0;
	}
}

void Texture::setColor( Uint8 red, Uint8 green, Uint8 blue )
{
	//Modulate texture rgb
	SDL_SetTextureColorMod( mTexture, red, green, blue );
}

void Texture::setBlendMode( SDL_BlendMode blending )
{
	//Set blending function
	SDL_SetTextureBlendMode( mTexture, blending );
}

void Texture::setAlpha( Uint8 alpha )
{
	//Modulate texture alpha
	SDL_SetTextureAlphaMod( mTexture, alpha );
}

/**
 * @brief Renders the texture to screen
 * @param renderer Supplied renderer to use
 * @param x The x-coordinate of the render position
 * @param y The y-coordinate of the render position
 * @param clip The area of the image which gets drawn
 */
void Texture::render( SDL_Renderer* renderer, int x, int y, const SDL_Rect* clip ) const
{
	//Set rendering space and render to screen
	SDL_Rect renderQuad = { x, y, mWidth, mHeight };

	//Set clip rendering dimensions
	if( clip != nullptr )
	{
		renderQuad.w = clip->w;
		renderQuad.h = clip->h;
	}

	//Render to screen
	SDL_RenderCopy( renderer, mTexture, clip, &renderQuad );
}

/**
 * @brief Renders the texture to screen with resizing abilities
 * @param renderer Supplied renderer to use
 * @param clip The area of the image which gets drawn
 * @param dest Destination area to draw to (including stretch or shrink)
 */
void Texture::render_resize(SDL_Renderer* renderer,const SDL_Rect* clip, const SDL_Rect* dest) const
{
    SDL_RenderCopy(renderer, mTexture, clip, dest);
}

int Texture::getWidth() const
{
	return mWidth;
}

int Texture::getHeight() const
{
	return mHeight;
}
