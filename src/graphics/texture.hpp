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
#ifndef TEXTURE_HPP_INCLUDED
#define TEXTURE_HPP_INCLUDED

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <string>

/**
 * @brief Stores and manages an SDL Texture
 * @todo Make a copy constructor, @c mTexture has to be handled via shared pointer then
 */

class Texture
{
	public:
		//Initializes variables
		Texture();

		//Deallocates memory
		~Texture();

		//Loads image at specified path
		bool loadFromFile(SDL_Renderer* renderer, std::string path );

		//Creates image from font string
		bool loadFromRenderedText( SDL_Renderer* renderer, std::string textureText, SDL_Color textColor, TTF_Font *font );

		//Deallocates texture
		void free();

		//Set color modulation
		void setColor( Uint8 red, Uint8 green, Uint8 blue );

		//Set blending
		void setBlendMode( SDL_BlendMode blending );

		//Set alpha modulation
		void setAlpha( Uint8 alpha );

		//Renders texture at given point
		void render( SDL_Renderer* renderer, int x, int y, const SDL_Rect* clip = nullptr) const;
		void render_resize(SDL_Renderer* renderer,const SDL_Rect* clip, const SDL_Rect* dest) const;

		//Gets image dimensions
		int getWidth() const;
		int getHeight() const;

	private:
		//The actual hardware texture
		SDL_Texture* mTexture; ///< The actual hardware texture

		//Image dimensions
		int mWidth;
		int mHeight;
};

#endif // TEXTURE_HPP_INCLUDED
