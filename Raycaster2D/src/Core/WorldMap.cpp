#include "WorldMap.h"
#include "Core/Input.h"
#include "Renderer/Renderer.h"

Cell WorldMap::s_map[MAP_WIDTH][MAP_HEIGHT];
std::vector<Edge> WorldMap::s_edges;
std::vector<glm::vec2> WorldMap::s_uniqueEdgePoints;
std::vector<std::tuple<float, float, float>> WorldMap::s_visibilityPolygonPoints;

std::vector<glm::vec2> WorldMap::s_newPoints;

int WorldMap::s_rayCount;

void WorldMap::Update()
{
    // Draw tiles
	if (Input::s_leftMouseDown && Renderer::s_renderMode == RENDER_MODE_UNLIT) {
		s_map[Input::s_gridX][Input::s_gridY].tile = Tile::WALL;
		BuildEdgeMapFromWorldMap();
	}
	if (Input::s_rightMouseDown && Renderer::s_renderMode == RENDER_MODE_UNLIT) {
		s_map[Input::s_gridX][Input::s_gridY].tile = Tile::DIRT;
		BuildEdgeMapFromWorldMap();
	}
	if (Input::s_keyPressed[HELL_KEY_N])
		NewMap();


	BuildEdgeMapFromWorldMap();

	s_visibilityPolygonPoints = CalculateVisibilityPolygon(Input::s_mouseWorldX, Input::s_mouseWorldY, 1.0f);
}

void WorldMap::NewMap()
{
    // Initialise map to dirt
    for (int x = 0; x < MAP_WIDTH; x++)
        for (int y = 0; y < MAP_HEIGHT; y++)
            s_map[x][y].tile = Tile::DIRT;
}

void WorldMap::LoadMap()
{
	std::string line;
	std::ifstream MyFile("res/WorldMap.txt");
	int y = 0;

	while (std::getline(MyFile, line)) {		
		for (int x = 0; x < line.length(); x++)	{
			
			int num; 
			std::stringstream ss;
			ss << line[x];
			ss >> num; 
			
			if (num == 0)
				s_map[x][y].tile = Tile::DIRT;
			else
				s_map[x][y].tile = Tile::WALL;

			std::cout << num;
		}
		std::cout << "\n";
		y++;
	}
	MyFile.close();
	BuildEdgeMapFromWorldMap();
}

void WorldMap::SaveMap()
{
	std::ofstream MyFile("res/WorldMap.txt");

	for (int y = 0; y < MAP_HEIGHT; y++) 
	{
		for (int x = 0; x < MAP_WIDTH; x++)
			MyFile << s_map[x][y].tile;
		MyFile << "\n";
	}
	MyFile.close();

}

void WorldMap::SaveInt(rapidjson::Value* object, std::string elementName, int number, rapidjson::Document::AllocatorType& allocator)
{
	rapidjson::Value name(elementName.c_str(), allocator);
	rapidjson::Value value(rapidjson::kObjectType);
	value.SetInt(number);
	object->AddMember(name, value, allocator);
}


void WorldMap::BuildEdgeMapFromWorldMap()
{
	// Clear "PolyMap"
	s_edges.clear();

	for (int x = 0; x < MAP_WIDTH; x++)
		for (int y = 0; y < MAP_HEIGHT; y++)
			for (int j = 0; j < 4; j++)
			{
				s_map[x][y].edge_exists[j] = false;
				s_map[x][y].edge_id[j] = 0;
			}



	Cell newMap[MAP_WIDTH][MAP_HEIGHT];
	//memcpy(newMap, s_map, MAP_WIDTH * MAP_HEIGHT * sizeof(Cell));
	for (int x = 0; x < MAP_WIDTH; x++)
		for (int y = 0; y < MAP_HEIGHT; y++)
		{
			for (int i = 0; i < 4; i++) {
				newMap[x][y].edge_exists[i] = s_map[x][y].edge_exists[i];
				newMap[x][y].edge_id[i] = s_map[x][y].edge_id[i];
			}
			newMap[x][y].tile = s_map[x][y].tile;
		}



	// Iterate through region from top left to bottom right
	for (int x = 0; x < MAP_WIDTH; x++)
		for (int y = 0; y < MAP_HEIGHT; y++)
		{
			Cell* cell = &s_map[x][y];
			Cell* n = GetCellPointer(x, y - 1);
			Cell* s = GetCellPointer(x, y + 1);
			Cell* w = GetCellPointer(x - 1, y);
			Cell* e = GetCellPointer(x + 1, y);

			// If this cell exists, check if it needs edges
			if (cell->IsObstacle())
			{
				// If this cell has no western neighbour, it needs a western edge
				if (w && !w->IsObstacle())
				{
					// It can either extend it from its northern neighbour if they have
					// one, or It can start a new one.
					if (n && n->edge_exists[WEST])
					{
						// Northern neighbour has a western edge, so grow it downwards
						s_edges[n->edge_id[WEST]].eY += GRID_SIZE;
						cell->edge_id[WEST] = n->edge_id[WEST];
						cell->edge_exists[WEST] = true;
					}
					else
					{
						// Northern neighbour does not have one, so create one
						Edge edge;
						edge.sX = x * GRID_SIZE;
						edge.sY = y * GRID_SIZE;
						edge.eX = x * GRID_SIZE;
						edge.eY = (y+1) * GRID_SIZE;
						// Add edge to Polygon Pool
						int edge_id = s_edges.size();
						s_edges.push_back(edge);
						// Update tile information with edge information
						cell->edge_id[WEST] = edge_id;
						cell->edge_exists[WEST] = true;
					}
				}
				
				// If this cell dont have an eastern neignbour, It needs a eastern edge
				if (e && !e->IsObstacle())
				{
					// It can either extend it from its northern neighbour if they have
					// one, or It can start a new one.
					if (n && n->edge_exists[EAST])
					{
						// Northern neighbour has one, so grow it downwards
						s_edges[n->edge_id[EAST]].eY += GRID_SIZE;
						cell->edge_id[EAST] = n->edge_id[EAST];
						cell->edge_exists[EAST] = true;
					}
					else
					{
						// Northern neighbour does not have one, so create one
						Edge edge;
						edge.sX = (x+1) * GRID_SIZE;
						edge.sY = y * GRID_SIZE;
						edge.eX = (x+1) * GRID_SIZE;
						edge.eY = (y + 1) * GRID_SIZE; 						
						// Add edge to Polygon Pool
						int edge_id = s_edges.size();
						s_edges.push_back(edge);
						// Update tile information with edge information
						cell->edge_id[EAST] = edge_id;
						cell->edge_exists[EAST] = true;
					}
				}
				// If this cell doesnt have a northern neignbour, It needs a northern edge
				if (n && !n->IsObstacle())
				{
					// It can either extend it from its western neighbour if they have
					// one, or It can start a new one.
					if (w && w->edge_exists[NORTH])
					{
						// Western neighbour has one, so grow it eastwards
						s_edges[w->edge_id[NORTH]].eX += GRID_SIZE;
						cell->edge_id[NORTH] = w->edge_id[NORTH];
						cell->edge_exists[NORTH] = true;
					}
					else
					{
						// Western neighbour does not have one, so create one
						Edge edge;
						edge.sX = (x) * GRID_SIZE;
						edge.sY = (y) * GRID_SIZE;
						edge.eX = (x + 1) * GRID_SIZE;
						edge.eY = (y) * GRID_SIZE;
						// Add edge to Polygon Pool
						int edge_id = s_edges.size();
						s_edges.push_back(edge);
						// Update tile information with edge information
						cell->edge_id[NORTH] = edge_id;
						cell->edge_exists[NORTH] = true;
					}
				}
				// If this cell doesnt have a southern neignbour, It needs a southern edge
				if (s && !s->IsObstacle())
				{
					// It can either extend it from its western neighbour if they have
					// one, or It can start a new one.
					if(w && w->edge_exists[SOUTH])
					{
						// Western neighbour has one, so grow it eastwards
						s_edges[w->edge_id[SOUTH]].eX += GRID_SIZE;
						cell->edge_id[SOUTH] = w->edge_id[SOUTH];
						cell->edge_exists[SOUTH] = true;
					}
					else
					{
						// Western neighbour does not have one, so I need to create one
						Edge edge;
						edge.sX = (x)*GRID_SIZE;
						edge.sY = (y+1)*GRID_SIZE;
						edge.eX = (x + 1) * GRID_SIZE;
						edge.eY = (y+1)*GRID_SIZE;
						// Add edge to Polygon Pool
						int edge_id = s_edges.size();
						s_edges.push_back(edge);
						// Update tile information with edge information
						cell->edge_id[SOUTH] = edge_id;
						cell->edge_exists[SOUTH] = true;
					}
				}
			}
		}
		FindUniqueEdgePoints();
}

void WorldMap::FindUniqueEdgePoints()
{
	s_uniqueEdgePoints.clear();
	for (auto& e : s_edges)
	{
		glm::vec2 startPoint(e.sX, e.sY);
		glm::vec2 endPoint(e.eX, e.eY);

		if (std::find(s_uniqueEdgePoints.begin(), s_uniqueEdgePoints.end(), startPoint) == s_uniqueEdgePoints.end())
			s_uniqueEdgePoints.push_back(startPoint);

		if (std::find(s_uniqueEdgePoints.begin(), s_uniqueEdgePoints.end(), endPoint) == s_uniqueEdgePoints.end())
			s_uniqueEdgePoints.push_back(endPoint);
	}
}

std::vector<std::tuple<float, float, float>> WorldMap::CalculateVisibilityPolygon(float ox, float oy, float radius)
{
	s_newPoints.clear();

	// Get rid of existing polygon
	std::vector<std::tuple<float, float, float>> visibilityPolygonPoints;

	s_rayCount = 0;

	float playerX = Input::s_mouseWorldX;
	float playerY = Input::s_mouseWorldY;

	// player bounds poly
	int lowerX = playerX - 200 * (1.777777);
	int upperX = playerX + 200 * (1.777777);
	int lowerY = playerY+- 200;
	int upperY = playerY - 200;


	// Duplicate the list of unique points (every "line end" in the world)
	/*std::vector < glm::vec2> duplicatePointList = s_uniqueEdgePoints;


	// Remove any points outside specified range
	for (int i = 0; i < duplicatePointList.size(); i++)
	{
		glm::vec2 point = duplicatePointList[i];

		if (point.x < lowerX
			|| point.x > upperX
			|| point.y < lowerY
			|| point.y > upperY
			)
			duplicatePointList.erase(duplicatePointList.begin() + i);
	}

	// SAME FOR EDGE LIST
	/*std::vector<Edge> survivingEdges = s_edges;

	for (int i = 0; i < survivingEdges.size(); i++)
	{
		Edge& edge = survivingEdges[i];
		float lowerEdgeX = std::min(edge.eX, edge.sX);

		if (lowerEdgeX < lowerX
			//|| point.x > upperX
			//|| point.y < lowerY
			//|| point.y > upperY
			)
		{
			survivingEdges.erase(survivingEdges.begin() + i);
			//std::cout << "REMOVED " << i << " " << lowerEdgeX << " is lower than " << lowerX << "\n";
		} 
	}
	*/


	//std::cout << "original edges: " << s_edges.size() << "\n";
	//std::cout << "survivingedges: " << survivingEdges.size() << "\n";

	//std::cout << "original points: " << s_uniqueEdgePoints.size() << "\n";
	//std::cout << "survivingpoints: " << duplicatePointList.size() << "\n\n";

	// For each edge in PolyMap
	//for (auto& e1 : s_edges)

	std::vector<glm::vec2> newPointList;
	for (glm::vec2 point : s_uniqueEdgePoints)
	{
		//newPointList.push_back(point);

	//	if (point.x > lowerX)
		{
			newPointList.push_back(point);
		}
	}

	std::vector<Edge> newEdgeList = s_edges;


	Edge leftRegionEdge;
	leftRegionEdge.sX = lowerX;
	leftRegionEdge.sY = lowerY;
	leftRegionEdge.eX = lowerX;
	leftRegionEdge.eY = upperY;

	Edge upperRegionEdge;
	upperRegionEdge.sX = lowerX;
	upperRegionEdge.sY = upperY;
	upperRegionEdge.eX = upperX;
	upperRegionEdge.eY = upperY;

	for (Edge edge : s_edges)
	{
		Edge newEdge;
		newEdge.sX = edge.sX;
		newEdge.eX = edge.eX;
		newEdge.sY = edge.sY;
		newEdge.eY = edge.eY;
		newEdgeList.push_back(newEdge);

		// now check if there are any edges sitting on LEFT REGION LINE
		if ((newEdge.sX < lowerX && newEdge.eX > lowerX) || (newEdge.sX > lowerX && newEdge.eX < lowerX)) {

			/*glm::vec2 newPoint = glm::vec2(lowerX, newEdge.eY);
			s_newPoints.push_back(newPoint);
			newPointList.push_back(newPoint);*/



			LineIntersectionResult result = CheckLineIntersection(edge, leftRegionEdge);
		//	if (result.intersectionFound)
	//			newPointList.push_back(glm::vec2(result.x, result.y));
		}
	}



	newEdgeList.push_back(leftRegionEdge);
	//newEdgeList.push_back(upperRegionEdge);
	

	for (auto& point : newPointList)
	{
		// Take the start point, then the end point (we could use a pool of
		// non-duplicated points here, it would be more optimal)
		//for (int i = 0; i < 2; i++)
		{
			float rdx, rdy;
			//rdx = (i == 0 ? e1.sX : e1.eX) - ox;
			//rdy = (i == 0 ? e1.sY : e1.eY) - oy;
			
			rdx = point.x - ox;
			rdy = point.y - oy;

			float base_ang = atan2f(rdy, rdx);

			float ang = 0;
			// For each point, cast 3 rays, 1 directly at point
			// and 1 a little bit either side
			for (int j = 0; j < 3; j++)
			{
				if (j == 1) continue;

				if (j == 0)	ang = base_ang - 0.0001f;
				if (j == 1)	ang = base_ang;
				if (j == 2)	ang = base_ang + 0.0001f;

				// Create ray along angle for required distance
				rdx = radius * cosf(ang);
				rdy = radius * sinf(ang);

				float min_t1 = INFINITY;
				float min_px = 0, min_py = 0, min_ang = 0;
				bool bValid = false;

				// Check for ray intersection with all edges
				for (auto& e2 : newEdgeList)
				{
			//		if (e2.sY < lowerY && e2.sY < lowerY)
			//			continue;

			//		if (e2.eX < lowerX && e2.sX < lowerX)
			//			continue;
	//				if (e2.sX < lowerX)
	//					continue;
						








					s_rayCount++;

					// Create line segment vector
					float sdx = e2.eX - e2.sX;
					float sdy = e2.eY - e2.sY;

					if (fabs(sdx - rdx) > 0.0f && fabs(sdy - rdy) > 0.0f)
					{
						// t2 is normalised distance from line segment start to line segment end of intersect point
						float t2 = (rdx * (e2.sY - oy) + (rdy * (ox - e2.sX))) / (sdx * rdy - sdy * rdx);
						// t1 is normalised distance from source along ray to ray length of intersect point
						float t1 = (e2.sX + sdx * t2 - ox) / rdx;

						// If intersect point exists along ray, and along line 
						// segment then intersect point is valid
						if (t1 > 0 && t2 >= 0 && t2 <= 1.0f)
						{
							// Check if this intersect point is closest to source. If
							// it is, then store this point and reject others
							if (t1 < min_t1)
							{
								min_t1 = t1;
								min_px = ox + rdx * t1;
								min_py = oy + rdy * t1;
								min_ang = atan2f(min_py - oy, min_px - ox);
								bValid = true;
							}
						}
					}
				}

				if (bValid)// Add intersection point to visibility polygon perimeter
				{
					//float ox, float oy

				/*	glm::vec2 difference = glm::vec2(ox, oy) - glm::vec2(min_px, min_py);
					difference = glm::normalize(difference);

					float scale = 15;
					min_px -= difference.x * scale;
					min_py -= difference.y * scale;*/

					
					visibilityPolygonPoints.push_back({ min_ang, min_px, min_py });
				}
			}
		}
	}

	// Sort perimeter points by angle from source. This will allow
	// us to draw a triangle fan.
	std::sort(
		visibilityPolygonPoints.begin(),
		visibilityPolygonPoints.end(),
		[&](const std::tuple<float, float, float>& t1, const std::tuple<float, float, float>& t2)
	{
		return std::get<0>(t1) < std::get<0>(t2);
	});

	return visibilityPolygonPoints;
}

LineIntersectionResult WorldMap::CheckLineIntersection(Edge lineA, Edge lineB)
{
	LineIntersectionResult result;
	result.intersectionFound = false;

	float p0_x = lineA.sX;
	float p0_y = lineA.sY;
	float p1_x = lineA.eX;
	float p1_y = lineA.eY;
	float p2_x = lineB.sX;
	float p2_y = lineB.sY;
	float p3_x = lineB.eX;
	float p3_y = lineB.eY;

	float s1_x, s1_y, s2_x, s2_y;
	s1_x = p1_x - p0_x;     s1_y = p1_y - p0_y;
	s2_x = p3_x - p2_x;     s2_y = p3_y - p2_y;

	float s, t;
	s = (-s1_y * (p0_x - p2_x) + s1_x * (p0_y - p2_y)) / (-s2_x * s1_y + s1_x * s2_y);
	t = (s2_x * (p0_y - p2_y) - s2_y * (p0_x - p2_x)) / (-s2_x * s1_y + s1_x * s2_y);

	if (s >= 0 && s <= 1 && t >= 0 && t <= 1)
	{
		// Collision detected
		result.x = p0_x + (t * s1_x);
		result.y = p0_y + (t * s1_y);
		result.intersectionFound = true;
	}

	return result;
}


Cell* WorldMap::GetCellPointer(int x, int y)
{
	if (x < 0 || y < 0 || x >= MAP_WIDTH || y >= MAP_HEIGHT)
		return nullptr;
	else
		return &s_map[x][y];
}
