extends Node3D

@onready var animator : AnimationPlayer = $AnimationPlayer

func _ready() -> void:
	animator.play("ScriptTest_baked")
