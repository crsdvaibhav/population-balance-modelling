from tkinter import *
import math
import random

root = Tk()

wh, ww = 600, 800

C = Canvas(root, height=wh, width=ww)
C.pack()

class Circle:
    def __init__(self, canvas, radius, posX, posY, color="yellow"):
        self.canvas = canvas
        self.radius = radius
        self.x = posX
        self.y = posY
        self.dx = random.randint(-50, 50) / 10.0  # Initial velocity in x direction
        self.dy = random.randint(-50, 50) / 10.0  # Initial velocity in y direction
        self.color = color
        self.id = self.create_circle()

    def create_circle(self):
        x0 = self.x - self.radius
        y0 = self.y - self.radius
        x1 = self.x + self.radius
        y1 = self.y + self.radius
        return self.canvas.create_oval(x0, y0, x1, y1, fill=self.color)

    def move(self):
        self.canvas.move(self.id, self.dx, self.dy)
        self.x += self.dx
        self.y += self.dy
        self.check_boundaries()

    def check_boundaries(self):
        x0, y0, x1, y1 = self.canvas.coords(self.id)
        if x0 <= 0 or x1 >= ww:
            self.dx = -self.dx
        if y0 <= 0 or y1 >= wh:
            self.dy = -self.dy

    def get_coords(self):
        return self.canvas.coords(self.id)

    def update_size(self, new_radius):
        self.radius = new_radius
        x0 = self.x - self.radius
        y0 = self.y - self.radius
        x1 = self.x + self.radius
        y1 = self.y + self.radius
        self.canvas.coords(self.id, x0, y0, x1, y1)

def check_collision(circle1, circle2):
    x1, y1, x2, y2 = circle1.get_coords()
    x3, y3, x4, y4 = circle2.get_coords()

    cx1, cy1 = (x1 + x2) / 2, (y1 + y2) / 2
    cx2, cy2 = (x3 + x4) / 2, (y3 + y4) / 2

    r1 = circle1.radius
    r2 = circle2.radius

    distance = math.sqrt((cx2 - cx1)**2 + (cy2 - cy1)**2)
    if distance <= r1 + r2:
        return True
    return False

def resolve_collision(circle1, circle2, e):
    x1, y1, x2, y2 = circle1.get_coords()
    x3, y3, x4, y4 = circle2.get_coords()

    cx1, cy1 = (x1 + x2) / 2, (y1 + y2) / 2
    cx2, cy2 = (x3 + x4) / 2, (y3 + y4) / 2

    dx1, dy1 = circle1.dx, circle1.dy
    dx2, dy2 = circle2.dx, circle2.dy

    # Vector from circle1 to circle2
    nx, ny = cx2 - cx1, cy2 - cy1
    length = math.sqrt(nx**2 + ny**2)
    nx /= length
    ny /= length

    # Relative velocity along the normal
    v1n = dx1 * nx + dy1 * ny
    v2n = dx2 * nx + dy2 * ny

    # Collision response
    v1n_after = (v1n * (circle1.radius - circle2.radius) + 2 * circle2.radius * v2n) / (circle1.radius + circle2.radius)
    v2n_after = (v2n * (circle2.radius - circle1.radius) + 2 * circle1.radius * v1n) / (circle1.radius + circle2.radius)

    # Apply coefficient of restitution
    v1n_after *= e
    v2n_after *= e

    # Update velocities
    circle1.dx += (v1n_after - v1n) * nx
    circle1.dy += (v1n_after - v1n) * ny
    circle2.dx += (v2n_after - v2n) * nx
    circle2.dy += (v2n_after - v2n) * ny

def animate(circles, e):
    global supersaturation

    for circle in circles:
        # Move each circle
        circle.move()

        # Growth: Increase radius based on supersaturation
        if supersaturation > critical_concentration:
            growth_rate = 0.1 * (supersaturation - critical_concentration)
            circle.update_size(circle.radius + growth_rate)

    # Check for breakage and agglomeration
    new_circles = []
    for i in range(len(circles)):
        for j in range(i + 1, len(circles)):
            if check_collision(circles[i], circles[j]):
                if random.random() < agglomeration_probability:
                    # Agglomeration: Merge circles
                    new_radius = math.sqrt(circles[i].radius**2 + circles[j].radius**2)
                    circles[i].update_size(new_radius)
                    circles[j].update_size(0)  # Mark for removal
                elif random.random() < breakage_probability:
                    # Breakage: Create two smaller circles
                    new_radius = circles[i].radius / 2
                    new_circles.append(Circle(C, new_radius, circles[i].x + random.randint(-10, 10), circles[i].y + random.randint(-10, 10)))
                    new_circles.append(Circle(C, new_radius, circles[i].x + random.randint(-10, 10), circles[i].y + random.randint(-10, 10)))
                    circles[i].update_size(0)  # Mark for removal

    # Remove marked circles
    circles = [c for c in circles if c.radius > 0]

    # Add new circles from breakage
    circles.extend(new_circles)

    # Update supersaturation
    supersaturation -= nucleation_rate
    supersaturation = max(supersaturation, critical_concentration)

    # Display supersaturation and particle count
    C.delete("info")
    C.create_text(50, 10, anchor="nw", text=f"Supersaturation: {supersaturation:.2f}", fill="black", tag="info")
    C.create_text(50, 30, anchor="nw", text=f"Particles: {len(circles)}", fill="black", tag="info")


    root.after(20, animate, circles, e)

# Simulation parameters
e = 0.8  # Coefficient of restitution
n = 100  # Number of initial circles
supersaturation = 1.0
critical_concentration = 0.05
nucleation_rate = 0.01
breakage_probability = 0.02
agglomeration_probability = 0.02

# Create circles
circles = [Circle(C, random.randint(5, 10), random.randint(10, ww - 10), random.randint(10, wh - 10)) for _ in range(n)]

# Start animation loop
animate(circles, e)

root.mainloop()
