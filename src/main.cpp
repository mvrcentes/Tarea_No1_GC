#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>
#include "color.h"
#include "vertex.h"
#include "framebuffer.h"

// Variables globales
Framebuffer framebuffer(800, 800);
Color clearColor(0, 0, 0); // Color base del framebuffer
Color currentColor(255, 255, 255); // Color actual

// Estructura para almacenar las coordenadas X e Y
struct Coordinates
{
    float x;
    float y;

    Coordinates(float xCoord, float yCoord) : x(xCoord), y(yCoord) {}
};

// Función para dibujar un vértice en las coordenadas especificadas
void Vertex2(const Coordinates &coords, const Color &color)
{
    // Lógica para dibujar el vértice en la pantalla
    framebuffer.set(static_cast<int>(coords.x), static_cast<int>(coords.y), color);
}

// Función para colocar un punto en la pantalla en las coordenadas especificadas
void punto(const Coordinates &coords, const Color &color)
{
    Vertex2(coords, color);
}

// Función para dibujar una línea usando el algoritmo de Bresenham
void line(int x1, int y1, int x2, int y2)
{
    int dx = abs(x2 - x1);
    int dy = abs(y2 - y1);
    int sx = (x1 < x2) ? 1 : -1;
    int sy = (y1 < y2) ? 1 : -1;
    int err = dx - dy;

    while (true)
    {
        punto(Coordinates(static_cast<float>(x1), static_cast<float>(y1)), currentColor);

        if (x1 == x2 && y1 == y2)
        {
            break;
        }

        int e2 = 2 * err;
        if (e2 > -dy)
        {
            err -= dy;
            x1 += sx;
        }
        if (e2 < dx)
        {
            err += dx;
            y1 += sy;
        }
    }
}

// Función para dibujar un polígono conectando los puntos
void drawPolygon(const std::vector<Coordinates> &points)
{
    for (size_t i = 0; i < points.size() - 1; ++i)
    {
        int x1 = static_cast<int>(points[i].x);
        int y1 = static_cast<int>(points[i].y);
        int x2 = static_cast<int>(points[i + 1].x);
        int y2 = static_cast<int>(points[i + 1].y);

        // Clamping de los puntos para que se mantengan dentro de la pantalla
        x1 = std::max(0, std::min(framebuffer.getWidth() - 1, x1));
        y1 = std::max(0, std::min(framebuffer.getHeight() - 1, y1));
        x2 = std::max(0, std::min(framebuffer.getWidth() - 1, x2));
        y2 = std::max(0, std::min(framebuffer.getHeight() - 1, y2));

        line(x1, y1, x2, y2);
    }

    // Dibujar la última línea que conecta el último punto con el primer punto
    int x1 = static_cast<int>(points.back().x);
    int y1 = static_cast<int>(points.back().y);
    int x2 = static_cast<int>(points.front().x);
    int y2 = static_cast<int>(points.front().y);

    // Clamping de los puntos para que se mantengan dentro de la pantalla
    x1 = std::max(0, std::min(framebuffer.getWidth() - 1, x1));
    y1 = std::max(0, std::min(framebuffer.getHeight() - 1, y1));
    x2 = std::max(0, std::min(framebuffer.getWidth() - 1, x2));
    y2 = std::max(0, std::min(framebuffer.getHeight() - 1, y2));

    line(x1, y1, x2, y2);
}

void fillPolygon(const std::vector<Coordinates> &vertices, const Color &fillColor)
{
    // Encontrar los límites del polígono
    float minX = std::numeric_limits<float>::max();
    float minY = std::numeric_limits<float>::max();
    float maxX = std::numeric_limits<float>::lowest();
    float maxY = std::numeric_limits<float>::lowest();

    for (const Coordinates &vertex : vertices)
    {
        minX = std::min(minX, vertex.x);
        minY = std::min(minY, vertex.y);
        maxX = std::max(maxX, vertex.x);
        maxY = std::max(maxY, vertex.y);
    }

    // Clamping de los límites para que se mantengan dentro de la pantalla
    int clampedMinX = std::max(0, static_cast<int>(minX));
    int clampedMinY = std::max(0, static_cast<int>(minY));
    int clampedMaxX = std::min(framebuffer.getWidth() - 1, static_cast<int>(maxX));
    int clampedMaxY = std::min(framebuffer.getHeight() - 1, static_cast<int>(maxY));

    // Rellenar el polígono con el color especificado
    for (int y = clampedMinY; y <= clampedMaxY; ++y)
    {
        std::vector<float> intersections;

        for (size_t i = 0; i < vertices.size(); ++i)
        {
            const Coordinates &currentVertex = vertices[i];
            const Coordinates &nextVertex = vertices[(i + 1) % vertices.size()];

            float x1 = currentVertex.x;
            float y1 = currentVertex.y;
            float x2 = nextVertex.x;
            float y2 = nextVertex.y;

            if ((y1 <= y && y2 > y) || (y1 > y && y2 <= y))
            {
                float intersectionX = (y - y1) * (x2 - x1) / (y2 - y1) + x1;
                intersections.push_back(intersectionX);
            }
        }

        std::sort(intersections.begin(), intersections.end());

        for (size_t i = 0; i < intersections.size(); i += 2)
        {
            int startX = std::max(static_cast<int>(intersections[i]), clampedMinX);
            int endX = std::min(static_cast<int>(intersections[i + 1]), clampedMaxX);

            for (int x = startX; x <= endX; ++x)
            {
                punto(Coordinates(static_cast<float>(x), static_cast<float>(y)), fillColor);
            }
        }
    }
}

// Función para renderizar la escena
void render(const std::vector<std::vector<Coordinates> > &polygons)
{
    framebuffer.clear(clearColor);

    // Colores de relleno para cada polígono
    Color fillColors[] = {
      Color(0, 255, 0),     // Verde
      Color(0, 0, 0)        // Negro
    };

    for (size_t i = 0; i < polygons.size(); ++i)
    {
        const std::vector<Coordinates> &polygon = polygons[i];

        // Dibujar el contorno del polígono
        drawPolygon(polygon);

        // Obtener el color de relleno correspondiente al polígono
        Color fillColor = fillColors[i % (sizeof(fillColors) / sizeof(fillColors[0]))];

        // Rellenar el polígono con el color correspondiente
        fillPolygon(polygon, fillColor);
    }
}

// Función para renderizar el buffer y escribir un archivo BMP
void renderBuffer()
{
    const int width = framebuffer.getWidth();
    const int height = framebuffer.getHeight();

    // Calcular el tamaño total del archivo BMP
    const int dataSize = width * height * 3;
    const int fileSize = 54 + dataSize;

    // Crear el encabezado del archivo BMP
    unsigned char header[54] = {
        'B', 'M',                                                                                                                                                                // Tipo de archivo
        static_cast<unsigned char>(fileSize), static_cast<unsigned char>(fileSize >> 8), static_cast<unsigned char>(fileSize >> 16), static_cast<unsigned char>(fileSize >> 24), // Tamaño del archivo
        0, 0,                                                                                                                                                                    // Reservado
        0, 0,                                                                                                                                                                    // Reservado
        54, 0, 0, 0,                                                                                                                                                             // Offset de los datos de la imagen
        40, 0, 0, 0,                                                                                                                                                             // Tamaño del encabezado del bitmap
        static_cast<unsigned char>(width), static_cast<unsigned char>(width >> 8), static_cast<unsigned char>(width >> 16), static_cast<unsigned char>(width >> 24),             // Ancho de la imagen
        static_cast<unsigned char>(height), static_cast<unsigned char>(height >> 8), static_cast<unsigned char>(height >> 16), static_cast<unsigned char>(height >> 24),         // Altura de la imagen
        1, 0,                                                                                                                                                                    // Planos de color (1)
        24, 0,                                                                                                                                                                   // Bits por píxel (24 - RGB)
        0, 0, 0, 0,                                                                                                                                                              // Compresión (no comprimido)
        static_cast<unsigned char>(dataSize), static_cast<unsigned char>(dataSize >> 8), static_cast<unsigned char>(dataSize >> 16), static_cast<unsigned char>(dataSize >> 24), // Tamaño de los datos de la imagen
        0, 0, 0, 0,                                                                                                                                                              // Resolución horizontal (no relevante)
        0, 0, 0, 0,                                                                                                                                                              // Resolución vertical (no relevante)
        0, 0, 0, 0,                                                                                                                                                              // Número de colores en la paleta (0 - sin paleta)
        0, 0, 0, 0                                                                                                                                                               // Colores importantes (todos)
    };

    // Abrir el archivo BMP para escritura en modo binario
    std::ofstream outFile("out.bmp", std::ios::binary);
    if (!outFile)
    {
        std::cout << "No se pudo abrir el archivo 'out.bmp' para escritura" << std::endl;
        return;
    }

    // Escribir el encabezado en el archivo
    outFile.write(reinterpret_cast<char *>(header), sizeof(header));

    // Escribir los datos de los píxeles en el archivo
    const std::vector<Color> &framebufferData = framebuffer.getFramebuffer();
    for (size_t i = 0; i < framebufferData.size(); ++i)
    {
        unsigned char pixelData[3] = {
            framebufferData[i].b,
            framebufferData[i].g,
            framebufferData[i].r};
        outFile.write(reinterpret_cast<char *>(pixelData), sizeof(pixelData));
    }

    // Cerrar el archivo
    outFile.close();

    std::cout << "Archivo BMP guardado como 'out.bmp'" << std::endl;
}

int main()
{
    // Definir los polígonos
    std::vector<std::vector<Coordinates> > polygons;

    std::vector<Coordinates> polygon4;
    polygon4.push_back(Coordinates(413, 177));
    polygon4.push_back(Coordinates(448, 159));
    polygon4.push_back(Coordinates(502, 88));
    polygon4.push_back(Coordinates(553, 53));
    polygon4.push_back(Coordinates(535, 36));
    polygon4.push_back(Coordinates(676, 37));
    polygon4.push_back(Coordinates(660, 52));
    polygon4.push_back(Coordinates(750, 145));
    polygon4.push_back(Coordinates(761, 179));
    polygon4.push_back(Coordinates(672, 192));
    polygon4.push_back(Coordinates(659, 214));
    polygon4.push_back(Coordinates(615, 214));
    polygon4.push_back(Coordinates(632, 230));
    polygon4.push_back(Coordinates(580, 230));
    polygon4.push_back(Coordinates(597, 215));
    polygon4.push_back(Coordinates(552, 214));
    polygon4.push_back(Coordinates(517, 144));
    polygon4.push_back(Coordinates(466, 180));
    polygons.push_back(polygon4);

    std::vector<Coordinates> polygon5;
    polygon5.push_back(Coordinates(682, 175));
    polygon5.push_back(Coordinates(708, 120));
    polygon5.push_back(Coordinates(735, 148));
    polygon5.push_back(Coordinates(739, 170));
    polygons.push_back(polygon5);

    // Renderizar la escena
    render(polygons);

    // Llamar a la función para renderizar el buffer
    renderBuffer();

    return 0;
}