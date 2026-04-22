#pragma once

class Attributes
{
public:
	Attributes();
	Attributes(int defense, int attack, int pace
		, int stamina, int passing, int tackle
		, int dribbling, int shooting, int physicality
		, int jumping, int reactionTime);

	int getDefense() const;
	int getAttack() const;
	int getPace() const;
	int getStamina() const;
	int getPassing() const;
	int getTackle() const;
	int getDribbling() const;
	int getShooting() const;
	int getPhysicality() const;
	int getJumping() const;
	int getReactionTime() const;
	int getGoalKeeping() const;

private:
	int _defense;
	int _attack;
	int _pace;
	int _stamina;
	int _passing;
	int _tackle;
	int _dribbling;
	int _shooting;
	int _physicality;
	int _jumping;
	int _reactionTime;
};
