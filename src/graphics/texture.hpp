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

#include <memory>
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <string>

/**
 * @brief Stores and manages an SDL Texture
 */

class Texture
{
	public:
		//Initializes variables
		Texture();

		//Deallocates texture
		void free();

		//Loads image at specified path
		bool loadFromFile(SDL_Renderer* renderer, std::string path );

		//Creates image from font string
		bool loadFromRenderedText( SDL_Renderer* renderer, std::string textureText, SDL_Color textColor, TTF_Font *font );

		//Set color modulation
		void setColor( Uint8 red, Uint8 green, Uint8 blue );

		//Set blending
		void setBlendMode( SDL_BlendMode blending );

		//Set alpha modulation
		void setAlpha( Uint8 alpha );

		//Renders texture at given point
		void render(int x, int y, const SDL_Rect* clip = nullptr) const;
		void render_resize(const SDL_Rect* clip, const SDL_Rect* dest) const;

		//Gets image dimensions
		int getWidth() const;
		int getHeight() const;

        struct Deleter {
            void operator()(SDL_Texture* p) {
                if(p != nullptr) {SDL_DestroyTexture(p);}
            }
        };

	private:
		//The actual hardware texture
		std::shared_ptr<SDL_Texture> mTexture; ///< The actual hardware texture

		SDL_Renderer* mRenderer;

		//Image dimensions
		int mWidth;
		int mHeight;
};

#endif // TEXTURE_HPP_INCLUDED
