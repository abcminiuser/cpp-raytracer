#include "Object.hpp"

#include "Vector.hpp"
#include "Scene.hpp"

Color Object::illuminate(const Scene& scene, const Vector& position)
{
	const Color		objectColor = colorAt(position);
	const Vector	normal = normalAt(position);

	Color finalColor = Palette::kBlack;

	for (const auto& l : scene.lights)
	{
		const float brightness = normal.dotProduct(l->position().subtract(position).unit());
		if (brightness > 0)
			finalColor = finalColor.add(l->illuminate(objectColor, position, brightness));
	}

	return finalColor;
}
