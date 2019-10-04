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

#include <algorithm>
#include <SDL_image.h>
#include <iostream>

Texture::Texture()
{
	//Initialize
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

    mRenderer = renderer;

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

	mTexture = std::shared_ptr<SDL_Texture>(newTexture, Texture::Deleter());

	//Return success
	return mTexture.get() != nullptr;
}
/**
 * @brief Creates a texture from a text
 * @param renderer Supplied renderer to use
 * @param textureText Text to be rendered
 * @param textColor Color of the text
 * @param font The font which is used
 * @return @c bool which indicates success or failure
 */
bool Texture::loadFromRenderedText( SDL_Renderer* renderer, std::string textureText, SDL_Color textColor, TTF_Font* font, Uint32 wrap )
{
	//Get rid of preexisting texture
	free();

	mRenderer = renderer;

	//Render text surface
	SDL_Surface* textSurface;
	if(wrap != 0) {
        textSurface = TTF_RenderUTF8_Blended_Wrapped( font, textureText.c_str(), textColor, wrap );
	}
	else {
        textSurface = TTF_RenderUTF8_Blended( font, textureText.c_str(), textColor );
	}

	if( textSurface != nullptr )
	{
		//Create texture from surface pixels
        mTexture = std::shared_ptr<SDL_Texture>(SDL_CreateTextureFromSurface( renderer, textSurface ), Texture::Deleter());
		if( mTexture.get() == nullptr )
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
	return mTexture.get() != nullptr;
}

/// Cleans up the hardware texture
void Texture::free()
{
        mTexture = nullptr;
        mRenderer = nullptr;
		mWidth = 0;
		mHeight = 0;
}

void Texture::setColor( Uint8 red, Uint8 green, Uint8 blue )
{
	//Modulate texture rgb
	SDL_SetTextureColorMod( mTexture.get(), red, green, blue );
}

void Texture::setBlendMode( SDL_BlendMode blending )
{
	//Set blending function
	SDL_SetTextureBlendMode( mTexture.get(), blending );
}

void Texture::setAlpha( Uint8 alpha )
{
	//Modulate texture alpha
	SDL_SetTextureAlphaMod( mTexture.get(), alpha );
}

/**
 * @brief Renders the texture to screen
 * @param renderer Supplied renderer to use
 * @param x The x-coordinate of the render position
 * @param y The y-coordinate of the render position
 * @param clip The area of the image which gets drawn
 */
void Texture::render( int x, int y, const SDL_Rect* clip ) const
{
	//Set rendering space and render to screen
	SDL_Rect renderQuad = { x, y, mWidth, mHeight };

	//Set clip rendering dimensions
	if( clip != nullptr )
	{
	    // Make sure a clip > base size doesn't stretch the destRect
		renderQuad.w = std::min(clip->w, mWidth);
		renderQuad.h = std::min(clip->h, mHeight);
	}

	//Render to screen
	SDL_RenderCopy( mRenderer, mTexture.get(), clip, &renderQuad );
}

/**
 * @brief Renders the texture to screen with resizing abilities
 * @param renderer Supplied renderer to use
 * @param clip The area of the image which gets drawn
 * @param dest Destination area to draw to (including stretch or shrink)
 */
void Texture::render_resize(const SDL_Rect* clip, const SDL_Rect* dest) const
{
    SDL_RenderCopy(mRenderer, mTexture.get(), clip, dest);
}

/// @todo Add documentation
void Texture::render_extra(int x, int y, const SDL_Rect* clip, double angle, bool x_flip, bool y_flip) const {
    //Set rendering space and render to screen
	SDL_Rect renderQuad = { x, y, mWidth, mHeight };

	//Set clip rendering dimensions
	if( clip != nullptr )
	{
		renderQuad.w = clip->w;
		renderQuad.h = clip->h;
	}
	SDL_RendererFlip flip = SDL_FLIP_NONE;
	if(x_flip) {flip = static_cast<SDL_RendererFlip>(flip | SDL_FLIP_HORIZONTAL);}
	if(y_flip) {flip = static_cast<SDL_RendererFlip>(flip | SDL_FLIP_VERTICAL);}
    SDL_RenderCopyEx(mRenderer, mTexture.get(), clip, &renderQuad, angle, nullptr, flip);
}

/// @todo Add documentation
void Texture::render_extra_resize(const SDL_Rect* clip, const SDL_Rect* dest, double angle, bool x_flip, bool y_flip) const {
	SDL_RendererFlip flip = SDL_FLIP_NONE;
	if(x_flip) {flip = static_cast<SDL_RendererFlip>(flip | SDL_FLIP_HORIZONTAL);}
	if(y_flip) {flip = static_cast<SDL_RendererFlip>(flip | SDL_FLIP_VERTICAL);}
    SDL_RenderCopyEx(mRenderer, mTexture.get(), clip, dest, angle, nullptr, flip);
}

int Texture::getWidth() const
{
	return mWidth;
}

int Texture::getHeight() const
{
	return mHeight;
}
