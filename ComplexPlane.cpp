#include "ComplexPlane.h"
#include <SFML/Graphics.hpp>
#include <complex>
#include <cmath>
#include <iostream>

ComplexPlane::ComplexPlane(int pixelWidth, int pixelHeight)
    : m_pixel_size(pixelWidth), m_aspectRatio(static_cast<float>(pixelHeight) / pixelWidth),
    m_plane_center(0.0f, 0.0f), m_plane_size(BASE_WIDTH, BASE_HEIGHT* m_aspectRatio),
    m_zoomCount(0), m_State(State::CALCULATING) {
    m_vArray.setPrimitiveType(sf::Points);
    m_vArray.resize(pixelWidth * pixelHeight);
}

void ComplexPlane::draw(sf::RenderTarget& target, sf::RenderStates states) const {
    target.draw(m_vArray, states);
}

void ComplexPlane::updateRender() {
    if (m_State == State::CALCULATING) {
        for (int j = 0; j < m_pixel_size; ++j) {
            for (int i = 0; i < m_pixel_size; ++i) {
                sf::Vector2f coord = mapPixelToCoords(sf::Vector2i(j, i));
                size_t iterations = countIterations(coord);

                uint8_t r, g, b;
                iterationsToRGB(iterations, r, g, b);

                m_vArray[j + i * m_pixel_size].position = sf::Vector2f(static_cast<float>(j), static_cast<float>(i));
                m_vArray[j + i * m_pixel_size].color = sf::Color(r, g, b);
            }
        }

        m_State = State::DISPLAYING;
    }
}

void ComplexPlane::zoomIn() {
    ++m_zoomCount;
    m_plane_size.x = BASE_WIDTH * pow(BASE_ZOOM, m_zoomCount);
    m_plane_size.y = BASE_HEIGHT * m_aspectRatio * pow(BASE_ZOOM, m_zoomCount);
    m_State = State::CALCULATING;
}

void ComplexPlane::zoomOut() {
    --m_zoomCount;
    m_plane_size.x = BASE_WIDTH * pow(BASE_ZOOM, m_zoomCount);
    m_plane_size.y = BASE_HEIGHT * m_aspectRatio * pow(BASE_ZOOM, m_zoomCount);
    m_State = State::CALCULATING;
}

void ComplexPlane::setCenter(sf::Vector2i mousePixel) {
    m_plane_center = mapPixelToCoords(mousePixel);
    m_State = State::CALCULATING;
}

void ComplexPlane::setMouseLocation(sf::Vector2i mousePixel) {
    m_mouseLocation = mapPixelToCoords(mousePixel);
}

sf::Vector2f ComplexPlane::mapPixelToCoords(sf::Vector2i pixel) {
    float x = ((float)pixel.x / m_pixel_size) * m_plane_size.x + (m_plane_center.x - m_plane_size.x / 2.0f);
    float y = ((float)pixel.y / m_pixel_size) * m_plane_size.y + (m_plane_center.y - m_plane_size.y / 2.0f);
    return sf::Vector2f(x, y);
}

size_t ComplexPlane::countIterations(sf::Vector2f coord) {
    std::complex<float> z(0, 0);
    std::complex<float> c(coord.x, coord.y);
    size_t i = 0;

    while (std::abs(z) < 2.0f && i < MAX_ITER) {
        z = z * z + c;
        ++i;
    }

    return i;
}

void ComplexPlane::iterationsToRGB(size_t count, uint8_t& r, uint8_t& g, uint8_t& b) {
    if (count == MAX_ITER) {
        r = g = b = 0;
    }
    else {

        float t = static_cast<float>(count) / MAX_ITER;
        float smoothCount = count + 1 - std::log(std::log(std::abs(std::complex<float>(0, 0))));

        float colorFactor = smoothCount - static_cast<int>(smoothCount); 

        r = static_cast<uint8_t>(9 * (1 - colorFactor) * colorFactor * colorFactor * colorFactor * 255);
        g = static_cast<uint8_t>(15 * (1 - colorFactor) * (1 - colorFactor) * colorFactor * colorFactor * 255);
        b = static_cast<uint8_t>(8.5 * (1 - colorFactor) * (1 - colorFactor) * (1 - colorFactor) * colorFactor * 255);
    }
}

void ComplexPlane::loadText(sf::Text& text) {
    std::stringstream ss;
    ss << "Center: (" << m_plane_center.x << ", " << m_plane_center.y << ")";
    text.setString(ss.str());
}
