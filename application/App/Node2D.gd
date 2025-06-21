extends Spatial

# Declare all variables at the top
var heightmap_image : Image
var terrain_width = 1024
var terrain_height = 1024
var height_scale = 50.0

func _ready():
	# Run external program (make sure path is correct)
		# Variables are amount of gridcells, amount of mesh layers, spread of force width, spread of force depth, screen width, screen height 
	OS.execute("main.exe", ['4','3', '3', '6', '1024', '1024'])
	
	
	var heightmap_texture = load("res://output.png")# Load the heightmap
	
	heightmap_image = heightmap_texture.get_data()
	heightmap_image.lock() # Must lock file otherwise it wont load
	
	# Check image dimensions
	print("Image size: ", heightmap_image.get_width(), "x", heightmap_image.get_height())
	if heightmap_image.get_width() < terrain_width or heightmap_image.get_height() < terrain_height:
		push_error("Heightmap image is smaller than terrain dimensions!")
		return
	
	# Create mesh
	var surface_tool = SurfaceTool.new()
	surface_tool.begin(Mesh.PRIMITIVE_TRIANGLES)
	
	# Generate terrain
	for x in range(terrain_width - 1):
		for z in range(terrain_height - 1):
			# Get heights, as its outputed as a RGB you can use any of the values
			var h1 = heightmap_image.get_pixel(x, z).r * height_scale
			var h2 = heightmap_image.get_pixel(x + 1, z).r * height_scale
			var h3 = heightmap_image.get_pixel(x, z + 1).r * height_scale
			var h4 = heightmap_image.get_pixel(x + 1, z + 1).r * height_scale
			
			# Create vertices
			var v1 = Vector3(x, h1, z)
			var v2 = Vector3(x + 1, h2, z)
			var v3 = Vector3(x, h3, z + 1)
			var v4 = Vector3(x + 1, h4, z + 1)
			
			# First triangle (v1, v2, v3)
			surface_tool.add_vertex(v1)
			surface_tool.add_vertex(v2)
			surface_tool.add_vertex(v3)
			
			# Second triangle (v2, v4, v3)
			surface_tool.add_vertex(v2)
			surface_tool.add_vertex(v4)
			surface_tool.add_vertex(v3)
	
	# Generate normals for proper lighting
	surface_tool.generate_normals()
	
	# Create and display mesh
	var mesh_instance = MeshInstance.new()
	mesh_instance.mesh = surface_tool.commit()
	add_child(mesh_instance)
	
	# Center the terrain
	mesh_instance.translation = Vector3(-terrain_width/2, 0, -terrain_height/2)
	
	# Unlock the image when done
	heightmap_image.unlock()
