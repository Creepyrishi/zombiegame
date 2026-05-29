#include "animation.h"

#include <stdlib.h>

SpriteAnimation CreateSpriteAnimation(Texture2D atlas, int framesPerSecond, Rectangle rectangles[], int length)
{
	SpriteAnimation spriteAnimation =
	{
		.atlas = atlas,
		.framesPerSecond = framesPerSecond,
		.timeStarted = GetTime(),
		.rectangles = NULL,
		.rectanglesLength = length
	};

	Rectangle* mem = (Rectangle*)malloc(sizeof(Rectangle) * length);
	if (mem == NULL)
	{
		TraceLog(LOG_FATAL, "No memory for CreateSpriteAnimation");
		spriteAnimation.rectanglesLength = 0;
		return spriteAnimation;
	}

	spriteAnimation.rectangles = mem;

	for (int i = 0; i < length; i++)
	{
		spriteAnimation.rectangles[i] = rectangles[i];
	}

	return spriteAnimation;
}

void DisposeSpriteAnimation(SpriteAnimation animation)
{
	free(animation.rectangles);
}

void ResetSpriteAnimation(SpriteAnimation* animation)
{
	animation->timeStarted = GetTime();
}

int IsSpriteAnimationFinished(SpriteAnimation animation)
{
	int index = (int)((GetTime() - animation.timeStarted) * animation.framesPerSecond);
	return index >= animation.rectanglesLength;
}

void DrawSpriteAnimationPro(SpriteAnimation animation, Rectangle dest, Vector2 origin, float rotation, Color tint)
{
	int index = (int)((GetTime() - animation.timeStarted) * animation.framesPerSecond) % animation.rectanglesLength;

	Rectangle source = animation.rectangles[index];
	DrawTexturePro(animation.atlas, source, dest, origin, rotation, tint);
}

SpriteAnimation CreateSpriteAnimationStrip(
    Texture2D atlas,
    int framesPerSecond,
    int frameWidth,
    int frameHeight,
    int frameCount
)
{
    Rectangle* rectangles = malloc(sizeof(Rectangle) * frameCount);

    if (rectangles == NULL)
    {
        TraceLog(LOG_FATAL, "No memory for CreateSpriteAnimationStrip");
        return (SpriteAnimation){
            .atlas = atlas,
            .framesPerSecond = framesPerSecond,
            .timeStarted = GetTime(),
            .rectangles = NULL,
            .rectanglesLength = 0
        };
    }

    for (int i = 0; i < frameCount; i++)
    {
        rectangles[i] = (Rectangle){
            i * frameWidth,
            0,
            frameWidth,
            frameHeight
        };
    }

    SpriteAnimation animation = CreateSpriteAnimation(
        atlas,
        framesPerSecond,
        rectangles,
        frameCount
    );

    free(rectangles);

    return animation;
}
