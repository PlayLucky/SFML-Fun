#include <SFML/Graphics.hpp>
#include <iostream>
#include <vector>

// Window dimensions
sf::Vector2i dimensions(800, 600);

// Values for Spirograph
float ring = 105;
float wheel = 63;
float hole = 21;

int paletteIndex = 10;

std::vector<sf::Color> palette = {
  sf::Color(33, 60, 132, 20),
  sf::Color(48, 88, 188, 20),
  sf::Color(195, 175, 227, 20),
  sf::Color(108, 86, 169, 20),
  sf::Color(230, 102, 175, 20),
  sf::Color(200, 85, 96, 20),
  sf::Color(236, 102, 93, 20),
  sf::Color(235, 73, 63, 20),
  sf::Color(208, 128, 91, 20),
  sf::Color(234, 180, 159, 20),
  sf::Color(237, 211, 83, 20)
};

sf::Color fgColor = palette[paletteIndex];
sf::Color bgColor = sf::Color::White;
sf::Color fxColor = sf::Color(200, 200, 200);

// Width of the lines
float lineWidth = 2;
float paintWidth = 4;

// Change speed with mouse wheel
float degreesPerFrame = 0.0;
float degreesPerFrameStep = 0.1;
float degreesPerFrameMax = 1;

float lengthMultiplier;

sf::Vector2f getRotatedVector(sf::RectangleShape shape, int point);
int slowestLineIndex(std::vector<float> degreesPerSecond);
void updateLines(std::vector<sf::RectangleShape>& lines);
void drawCircleOutline(sf::RenderWindow& window, int x, int y, float r, sf::Color color);

int main() {
  lengthMultiplier = (dimensions.y/2 - 1)/ring;
  // Pixelarray for drawn pixels
  sf::VertexArray paint(sf::TrianglesStrip);

  // Create window
  sf::RenderWindow window(sf::VideoMode(dimensions.x, dimensions.y), "Spirograph");

  std::vector<sf::RectangleShape> lines(3);

  for(uint i = 0; i < lines.size(); i++) {
    lines[i] = sf::RectangleShape();
    lines[i].rotate(90);
    lines[i].setFillColor(fxColor);
  }
  updateLines(lines);


  // Move first line to screen center
  lines[0].setPosition(sf::Vector2f(dimensions.x/2, dimensions.y/2));

  bool linesVisible = true;

  // MAINLOOP
  while(window.isOpen()) {
    // Check for events
    sf::Event event;
    while(window.pollEvent(event)) {
      // Check if event is one of the following types
      switch(event.type) {
        // Close window
        case sf::Event::Closed:
          window.close();
          break;
          // Scrollwheel to change degreesPerFrame
        case sf::Event::MouseWheelScrolled:
          {
            // Change degreesPerFrame by amount scrolled
            degreesPerFrame += event.mouseWheelScroll.delta * degreesPerFrameStep;
            // Check minimum and maximum
            if(degreesPerFrame > degreesPerFrameMax) degreesPerFrame = degreesPerFrameMax;
            if(degreesPerFrame < 0) degreesPerFrame = 0;

            // Hide lines if degreesPerFrame is 0
            if(degreesPerFrame == 0) linesVisible = false;
            else linesVisible = true;
            break;
          }
        case sf::Event::KeyPressed:
          switch(event.key.code) {
            case sf::Keyboard::Space:
              linesVisible = !linesVisible;
              break;
            case sf::Keyboard::Return:
              {
                std::cout << "Please input: ring wheel hole" << std::endl;
                std::cin >> ring;
                std::cin >> wheel;
                std::cin >> hole;
                updateLines(lines);
                break;
              }
            case sf::Keyboard::R:
              {
                std::cout << "Please input: ring" << std::endl;
                std::cin >> ring;
                updateLines(lines);
                break;
              }
            case sf::Keyboard::W:
              {
                std::cout << "Please input: wheel" << std::endl;
                std::cin >> ring;
                updateLines(lines);
                break;
              }
            case sf::Keyboard::H:
              {
                std::cout << "Please input: hole" << std::endl;
                std::cin >> hole;
                updateLines(lines);
                break;
              }
            case sf::Keyboard::C:
              {
                std::cout << "Please input: r g b" << std::endl;
                int r, g, b;
                std::cin >> r >> g >> b;
                fgColor = sf::Color(r, g, b);
                break;
              }
            case sf::Keyboard::N:
              fgColor = palette[--paletteIndex];
              hole -= 2;
              updateLines(lines);
              break;
            default: break;
          }
        default:
          break;
      }
    }

    // LOGIC

    if(linesVisible != 0) {
      // Rotate each line
      lines[0].rotate(degreesPerFrame);
      lines[1].rotate(degreesPerFrame * -(ring - wheel) / wheel);
      lines[2].setRotation(lines[1].getRotation());
      // Move each line
      for(uint i = 1; i < lines.size(); i++) {
        // Move position to the end of the previous line
        lines[i].setPosition(getRotatedVector(lines[i-1], 1));
      }
      sf::Vector2f pos = getRotatedVector(lines.back(), 1);
      paint.append(sf::Vertex(sf::Vector2f(pos.x-paintWidth, pos.y), fgColor));
      paint.append(sf::Vertex(sf::Vector2f(pos.x+paintWidth, pos.y), fgColor));
      paint.append(sf::Vertex(sf::Vector2f(pos.x, pos.y-paintWidth), fgColor));
      paint.append(sf::Vertex(sf::Vector2f(pos.x, pos.y+paintWidth), fgColor));
    }

    // RENDER

    // Fill the window with background color
    window.clear(bgColor);

    // Draw each line
    window.draw(paint);
    if(linesVisible) {
      window.draw(lines[1]);
      sf::Vector2f pos(getRotatedVector(lines[1], 0));
      drawCircleOutline(window, pos.x, pos.y, lines[1].getSize().x, fxColor);
    }

    drawCircleOutline(window, dimensions.x/2, dimensions.y/2, ring * lengthMultiplier, fxColor);

    // Swap buffers to show the image to the screen
    window.display();
  }

  // Program ended without errors
  return 0;
}

sf::Vector2f getRotatedVector(sf::RectangleShape shape, int point) {
  // You can't just use the shape.getPoint() method, because it doesn't
  // take the transformations (rotations) into consideration
  return shape.getTransform().transformPoint(shape.getPoint(point));
}

void updateLines(std::vector<sf::RectangleShape>& lines) {
  float hole_temp = hole * 2.3 + 10;
  lines[0].setSize(sf::Vector2f((ring-wheel) * lengthMultiplier, lineWidth));
  lines[1].setSize(sf::Vector2f(wheel * lengthMultiplier, lineWidth));
  lines[2].setSize(sf::Vector2f(-hole_temp * lengthMultiplier, lineWidth));
}

void drawCircleOutline(sf::RenderWindow& window, int x, int y, float r, sf::Color color) {
  sf::CircleShape circle(r, 100); 
  circle.setOrigin(r, r);
  circle.setPosition(x, y);
  circle.setFillColor(sf::Color::Transparent);
  circle.setOutlineThickness(1);
  circle.setOutlineColor(color);
  window.draw(circle);
}
