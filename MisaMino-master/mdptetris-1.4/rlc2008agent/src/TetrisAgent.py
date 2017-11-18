#################################################################################################
#
# MAIA Tetris Agent for RL competition
#
# Optimization of weights for the good criterion (for various reward distributions)
# Automatically tuned strategy (for better adaptiveness to unexpected things!)
# 

##################################################################################################
# Display info about the observation/reward/macros/actions + comparison with the internal model
verbose=False

# Display much more info (macros considered, evaluation, functions entered)
#
debug=False

# Save the sequence of pieces into a file (put a non-positive number no to record)
save_pieces=0
##################################################################################################

# Learning time for evaluating each strategy
Learning_time=50000
pre_learning=2000     # the first step don't count in the evaluation

#################################################################
# Option to be discrete on the Proving Leader Board.... (put to -1 to disable)
SUICIDE_TIME=-1
SUICIDE_MACRO=[4]*100 # nop*100

##################################################################################################
sum_expected=0.0

# put the PATH here if necessary
prog="mdptetris_rlc"


# Strategies are list of (eval function number,forcetetris value)
#

strategies=[ \
    (13,8), (-1,0)
    ]

strategy_number=len(strategies)
strategy_index=0
strategy=0
forcetetris_height=0
reward_strategy=[]

# For speed-up
import psyco

# general modules
import sys, copy
from rlglue.agent.Agent import Agent
from rlglue.types import Action
from rlglue.types import Observation

# for launching mdptetris_rlc
import os


def command(cmd):
    print cmd
    sys.stdout.flush()
    os.system(cmd)



class TetrisAgent(Agent):
	"MAIA Tetris Agent"


########################### Variables

	# Game dimensions
	height=0
	width=0

# pieces in all their orientations 
	pieces = [ \
		#I
		[ 2, [ [[0,0,0,0], \
			[1,1,1,1], \
			[0,0,0,0], \
			[0,0,0,0]], \
		       [[0,0,1,0], \
			[0,0,1,0], \
			[0,0,1,0], \
			[0,0,1,0]] ] ], \
		#O
		[ 1, [ [[0,0,0,0], \
			[0,1,1,0], \
			[0,1,1,0], \
			[0,0,0,0]] ] ], \
		#T
		[ 4, [ [[0,0,1,0], \
			[0,1,1,0], \
			[0,0,1,0], \
			[0,0,0,0]], \
		       [[0,0,0,0], \
			[0,1,1,1], \
			[0,0,1,0], \
			[0,0,0,0]], \
		       [[0,0,1,0], \
			[0,0,1,1], \
			[0,0,1,0], \
			[0,0,0,0]], \
		       [[0,0,1,0], \
			[0,1,1,1], \
			[0,0,0,0], \
			[0,0,0,0]] ] ], \
		#Z
		[ 2, [ [[0,0,0,0], \
			[0,1,1,0], \
			[0,0,1,1], \
			[0,0,0,0]], \
		       [[0,0,0,0], \
			[0,0,1,0], \
			[0,1,1,0], \
			[0,1,0,0]] ] ], \
		#S
		[ 2, [ [[0,0,0,0], \
			[0,0,1,1], \
			[0,1,1,0], \
			[0,0,0,0]], \
		       [[0,0,0,0], \
			[0,1,0,0], \
			[0,1,1,0], \
			[0,0,1,0]] ] ], \
		#J
		[ 4, [ [[0,0,0,0], \
			[0,1,1,1], \
			[0,0,0,1], \
			[0,0,0,0]], \
		       [[0,0,1,1], \
			[0,0,1,0], \
			[0,0,1,0], \
			[0,0,0,0]], \
		       [[0,1,0,0], \
			[0,1,1,1], \
			[0,0,0,0], \
			[0,0,0,0]], \
		       [[0,0,1,0], \
			[0,0,1,0], \
			[0,1,1,0], \
			[0,0,0,0]] ] ], \
		#L
		[ 4, [ [[0,0,0,1], \
			[0,1,1,1], \
			[0,0,0,0], \
			[0,0,0,0]], \
		       [[0,1,1,0], \
			[0,0,1,0], \
			[0,0,1,0], \
			[0,0,0,0]], \
		       [[0,0,0,0], \
			[0,1,1,1], \
			[0,1,0,0], \
			[0,0,0,0]], \
		       [[0,0,1,0], \
			[0,0,1,0], \
			[0,0,1,1], \
			[0,0,0,0]] ] ] \
		]

	# contains the same information as piece but is optimized: will be initialized automatically by pieces2_init()
	pieces2=[]


	# first last x/y in the above arrays: will be initialized automatically by pieces_xy_init()
	pieces_x=[]
	pieces_y=[]

	# the board + a buffer (for one step ahead evaluation)
	board=[]
	board2=[]

	# current piece
	piecenb=0
	x,y,orient=0,0,0
	piecenb=0

	# list of macros: will be initialized automatically by macros_init()
	macros=[]
	forcetetris_macros=[]

	# current macro
	current_macro=[]
	current_macro_index=0

	# stats: internal count of the number of lines (0,1,2,3,4), associated rewards, heights
	last_number_of_removed_lines=0
	number_of_lines=[]
	rewards_for_lines=[]
	heights=[]

	# gives the index where the first height is found (optimization)
	first_height=0

	# 
	sum_rewards=0
	this_mdp_sum_rewards=0
	total_sum_rewards=0
	step_number=0
	this_mdp_step_number=0
	mdp_count=0
	episode_count=0

	# precomputations of range(width) and range(height)
	range_width=[]
	range_height=[]


	# array of size self.height (initialized at start - optimization)
	rows_with_holes_list = []


	# heights for each column
	column_height=[]

	# stats for the pieces
	piecenb_stat=[]
	piece_count=0

	# file for recording the pieces
	pieces_file=0

        w=[]
	
############################### Methods


	def pieces2_init(self):
		self.pieces2=[]
		for i in range(len(self.pieces)):
			self.pieces2.append([])
			nbor=self.pieces[i][0]
			for j in range(nbor):
				self.pieces2[i].append([])
				p=self.pieces[i][1][j]
				for x in range(4):
					for y in range(4):
						if p[y][x]==1:
							self.pieces2[i][j].append((x,y))		
			
	def pieces_xy_init(self):		
		"Initializes the array pieces_x and pieces_y that contain the dimensions"
		l2x,l2y=[],[]
		for i in range(len(self.pieces)):			
			nbor=self.pieces[i][0]
			lx,ly=[],[]
			for j in range(nbor):				
				xmin,ymin=4,4
				xmax,ymax=-1,-1
				p=self.pieces[i][1][j]		
				for (x,y) in self.pieces2[i][j]:
					if x<xmin:
						xmin=x
					if x>xmax:
						xmax=x
					if y<ymin:
						ymin=y
					if y>ymax:
						ymax=y
				lx.append([xmin,xmax])
				ly.append([ymin,ymax])
			l2x.append(lx)
			l2y.append(ly)
		self.pieces_x=l2x
		self.pieces_y=l2y


	def generate_macro_action(self, piecenb, nbtr, nbrot):
		"generates a list of actions from numbers of translations and rotations"
		# first action is a translation (or nothing)
		if nbtr>0:
			l=[1] # right			
		elif nbtr<0:
			l=[0] # left
		elif nbrot!=0 and (piecenb==0 or piecenb==5):
			l=[4] # nop (when piece is I or J: need to wait before first rotation)
		else:
			l=[] # drop directly
		# following actions are rotations
		if nbrot>0:
			l=l+([2]*nbrot) # rotate left
		elif nbrot<0:
			l=l+([3]*(-nbrot)) # rotate right
		# then the remaining transitions
		if nbtr>0:
			l=l+([1]*(nbtr-1)) # right
		elif nbtr<0:
			l=l+([0]*(-nbtr-1)) # left
		l.append(5) # drop action at the end
		return l




	def macros_init(self):
		"Initializes the array of macros for all pieces"
		self.macros=[]
		self.forcetetris_macros=[]
		for piecenb in range(len(self.pieces)):
			self.macros.append([])
			self.forcetetris_macros.append([])
			x=self.width/2-2                               # always in the middle
			y=-self.pieces_y[self.piecenb][self.orient][0] # always on top
			nbor=self.pieces[piecenb][0]		
			for ori in [[0],[-1,0],[-2,-1,0,1]][nbor/2]:         # 1->[0] 2->[-1,0] 4->[-2,-1,0,1]
				px=self.pieces_x[piecenb][(ori+nbor)%nbor]						
				for tr in range( -(x+px[0]), self.width-(x+px[1]) ):
					self.macros[piecenb].append( self.generate_macro_action(piecenb,tr,ori) )
					if (tr<self.width-(x+px[1])-1):# or (piecenb,ori)==(0,-1) or (piecenb,ori)==(6,1): # for forcetetris: consider the last column only if vertical I or 'upside down L' <- fix: only consider the last column...
						self.forcetetris_macros[piecenb].append(True)
					else:
						self.forcetetris_macros[piecenb].append(False)
		
		

	def collision(self,x,y,orient):
		"check whether the current piece at position x,y and orientation orient collides with the current board"
		px=self.pieces_x[self.piecenb][orient]
		py=self.pieces_y[self.piecenb][orient]

		if x+px[0]<0 or x+px[1]>=self.width: # out of the board (left side or right side)
			return 1
		if y+py[0]<0 or y+py[1]>=self.height:# out of the board (ex: illegal rotation on top or touching the ground)
			return 1		
		if y+py[1]>=self.first_height: # no test necessary if the piece is higher than the top of the board
			for (dx,dy) in self.pieces2[self.piecenb][orient]:
				if self.board[x+dx][y+dy]==1:
					return 1	
		return 0


	def put_piece_on_board(self,piecenb,x,y,orient,value):
		"Put the piece piecenb on the current board at position x,y and orientation orient (called when such a piece touches the ground)"
		for (dx,dy) in self.pieces2[piecenb][orient]:
			self.board[x+dx][y+dy]=value


	def try_action(self, x, y, orient, action):
		"Simulates one micro action on the current board for the current piece from x,y,orient - the first returned argument is 1 if the piece is on the ground and 0 else"
		nx,ny,norient=x,y,orient
		 		
		if action==0:   # Left
			nx=x-1			
		elif action==1: # Right
       			nx=x+1
		elif action==2: # Rotate left
			nb_orient=self.pieces[self.piecenb][0]
			norient=(orient+1)%nb_orient
		elif action==3: # Rotate right
			nb_orient=self.pieces[self.piecenb][0]
			norient=(orient+nb_orient-1)%nb_orient		
		elif action==5: # drop
			while(self.collision(x, ny, orient)==0):
				ny=ny+1
			ny=ny-1
		if self.collision(nx, ny, norient)!=0: # if the move is not allowed
			nx,ny,norient=x,y,orient                       # then cancel it
		ny=ny+1 # make it go downward
		if self.collision(nx, ny, norient)!=0:
			return 1,nx,ny-1,norient # reached the ground!
		else:
			return 0,nx,ny,norient   # still room to go down



	def print_board(self, board):
		for i in self.range_height:
			line=""
			for j in self.range_width:
				if board[j][i]==0:
					line += "."
				else:
					line += "X"
			print(line)
	
					
					
	def print_observation(self, observation):
		bug=0
		array=observation.intArray
		pieces=["I","O","T","Z","S","J","L"] 
		print "current piece:",pieces[self.piecenb]
		addcurrentpiece = 1 - self.collision(self.x,self.y,self.orient) # don't add the current piece if it collides with the board (useful for gameover situations)
		if addcurrentpiece==1:
			self.put_piece_on_board(self.piecenb,self.x,self.y,self.orient,-1) # show the current piece
		for i in self.range_height:
			line=""
			line2=""
			for j in self.range_width:
				if array[j+i*self.width]==1:
					line=line+"X"
				else:
					line=line+"."
				if self.board[j][i]==1:
					line2=line2+"X"
				elif self.board[j][i]==-1:
					line2=line2+"X"
				else:				
					line2=line2+"."				
			print line," ",line2
			if line!=line2:            # there should not be any difference between our prediction and the observation !!!
				bug=1
		print "wall height=",self.height-self.first_height
		if addcurrentpiece==1:
			self.put_piece_on_board(self.piecenb,self.x,self.y,self.orient,0) # remove it
		sys.stdout.flush()
		if bug==1:
			print "PROBLEM: model difference !!!"
			sys.exit()
			
				

	def get_next_piece(self, observation):
		"Look at the new piece in the observation vector and put it in our simulator"
		if verbose:
			print "Getting "+str(self.piece_count)+"th piece (",self.this_mdp_step_number,")"
		self.piecenb=list(observation.intArray[-9:-2]).index(1) # piecenb coded in the observation vector
		self.piecenb_stat[self.piecenb]+=1
		self.piece_count+=1
		
		if save_pieces>0:
			if self.piece_count<=save_pieces:
				self.pieces_file.write(str(self.piecenb)+"\n")
			else:
				self.pieces_file.close()
				sys.exit() # I am done
			
		
		self.orient=0
		self.x=self.width/2-2                               # always in the middle
		self.y=-self.pieces_y[self.piecenb][self.orient][0] # always on top
		self.need_new_piece=0


	def remove_lines(self,l):
		"Remove the lines contained in an ordered list" 
		for yr in l:
			empty=0
			y=yr
			while(empty==0):  # this loop removes all the lines above y
				empty=1
				for x in self.range_width:
					if y>=1 and self.board[x][y-1]==1:
						self.board[x][y]=1
						empty=0
					else:
						self.board[x][y]=0
				y=y-1											

	def check_lines_for_removal(self,ymin,ymax):
		"Returns an ordered list containing the lines to remove"
		l=[]
		y=ymin
		while y<=ymax:
			full=1
			for x in self.range_width:
				if self.board[x][y]!=1:
					full=0
					break
			if full==1:
				l.append(y)
			y += 1
		return l
		

	def set_action_for_rlglue(self):
		self.action.intArray = [self.current_macro[self.current_macro_index]]
		self.action.doubleArray = []
		self.current_macro_index = self.current_macro_index+1


	def do_action(self,observation):					
				
		# Choose an action
		self.set_action_for_rlglue()

		# Do it (in our simulator) then return it (for their simulator)
		actions=["left", "right", "rotate_left", "rotate_right", "nop", "drop"]
		my_action=self.action.intArray[0]
		if verbose:
			print "Action =",actions[my_action]		
		#print "x=",self.x,"y=",self.y,"orient=",self.orient
		ground,self.x,self.y,self.orient=self.try_action(self.x,self.y,self.orient,my_action)
		if ground==1: # piece has reached the ground

			# update the board
			self.put_piece_on_board(self.piecenb,self.x,self.y,self.orient,1)			
			self.need_new_piece=2			
			py=self.pieces_y[self.piecenb][self.orient]
			l=self.check_lines_for_removal(self.y+py[0],self.y+py[1])
			nb_lines_removed=len(l)
			self.remove_lines(l)
			self.first_height = min(self.first_height,self.y+py[0])+nb_lines_removed
			
			# doing stats on the MDP
			self.number_of_lines[nb_lines_removed] += 1
			self.last_nb_lines_removed=nb_lines_removed  #  for the coming reward (see agent_step)
			self.heights[self.height-self.first_height] += 1						
			
		#print "x=",self.x,"y=",self.y,"orient=",self.orient
		return self.action					



	

	def try_macro_action(self,l):
		"try macro action l from current board and current piece on top (original orient should be 0). Returns the final position (should be on the ground as all macros should finish by drop)"
		orient=0
		x=self.x
		y=self.y		
		for action in l:
			ground,x,y,orient=self.try_action(x, y, orient, action)
			if ground==1:
				break   # touches the ground
		return x,y,orient
			

	def copy_board_to_buffer(self):
		board=self.board
		board2=self.board2
		for yy in self.range_height:
			for xx in self.range_width:
				board2[xx][yy] = board[xx][yy]

	def restore_board_from_buffer(self):
		board=self.board
		board2=self.board2
		for yy in self.range_height:
			for xx in self.range_width:
				board[xx][yy] = board2[xx][yy]

	def evaluate_macro(self, l):
		
		if debug:
			print "considering macro:",
			actions=["left", "right", "rotate_left", "rotate_right", "nop", "drop"]
			for i in l:
				print actions[i],
			print  
					
		x2,y2,orient2 = self.try_macro_action(l)

		self.put_piece_on_board(self.piecenb,x2,y2,orient2,1)

		p = self.pieces[self.piecenb][1][orient2]
		px = self.pieces_x[self.piecenb][orient2]
		py = self.pieces_y[self.piecenb][orient2]

		# number of cells removed
		r = self.check_lines_for_removal(y2+py[0],y2+py[1])
		nb_cell_removed=0
		for y in r:
			x=px[0]
			while x<=px[1]:
				if p[y-y2][x]==1:
					nb_cell_removed=nb_cell_removed+1
				x += 1
		self.remove_lines(r)

		# number of lines removed
		lines_removed = len(r)

		# new height
		first_height2 = min(self.first_height,y2+py[0])+lines_removed

		if debug:
			self.print_board(self.board)
			print "Height=",self.height-first_height2		

		# landing height
		landing_height = self.height-(y2+(py[1]+py[0])/2.0)-1

		# eroded piece cells
		eroded_piece_cells = lines_removed * nb_cell_removed

		# row transitions
		row_transitions = 2*first_height2 
		y = first_height2
		while y<self.height:
			if self.board[0][y]==0:
				row_transitions += 1
			x = 1
			while x<self.width:
				if self.board[x][y]!=self.board[x-1][y]:
					row_transitions += 1
				x += 1
			if self.board[self.width-1][y]==0:
				row_transitions += 1
			y += 1

		# column transitions
		column_transitions = 0
		for x in self.range_width:
			y = max(1,first_height2)
			while y<self.height:
				if self.board[x][y]!=self.board[x][y-1]:
					column_transitions += 1
				y += 1
			if self.board[x][self.height-1]==0:
				column_transitions += 1
		
		# column_heights, hole number, hole depths and rows with holes, diversity
		hole_number = 0
                div_dm2 = 0
                div_dm1 = 0
                div_d0 = 0
                div_d1 = 0
                div_d2 = 0
		for y in self.range_height:
			self.rows_with_holes_list[y] = 0
		rows_with_holes = 0		
		hole_depths = 0		
		for x in self.range_width:
			y = first_height2
			while y<self.height and self.board[x][y]==0: # find the first full cell of column x
				y += 1
			self.column_height[x] = self.height-y
                        if x>0: # computing diversity
                            tmp = self.column_height[x]-self.column_height[x-1]
                            if tmp == -2:
                                div_dm2 = 1
                            elif tmp == -1:
                                div_dm1 = 1
                            elif tmp == 0:
                                div_d0 = 1
                            elif tmp == 1:
                                div_d1 = 1
                            elif tmp == 2:
                                div_d2 = 1
			y += 1
			full_cells_above=1
			while y<self.height:		
				if self.board[x][y]==0:    # for each free cell under (= for each hole)
					# hole_number
					hole_number += 1
					# rows with holes
					if self.rows_with_holes_list[y]==0:
						self.rows_with_holes_list[y]=1
						rows_with_holes += 1
					# hole depth
					hole_depths += full_cells_above
					full_cells_above = 0
				else:                      # for each full cell
					full_cells_above += 1
				
				y +=1
                diversity = div_dm2 + div_dm1 + div_d0 + div_d1 + div_d2 # between 0 and 5 

                # max_height
		max_height=self.height-first_height2
                
		# well sums 
		well_sums=0
		y = first_height2 #### ???should be: y = first_height2 (reproducing a BUG of our weight optimizer)
		while y<self.height: # First column
			if self.board[0][y]==0 and self.board[1][y]==1:
				well_sums += 1
				yy = y+1
				while yy<self.height and self.board[0][yy]==0:
					well_sums += 1
					yy += 1
			y += 1
		x = 1
		while x<self.width-1:
			y = first_height2 #### ???should be: y = first_height2 (reproducing a BUG of our weight optimizer)
			while y<self.height: # Inner columns
				if self.board[x-1][y]==1 and self.board[x][y]==0 and self.board[x+1][y]==1:
					well_sums += 1
					yy = y+1
					while yy<self.height and self.board[x][yy]==0:
						well_sums += 1
						yy += 1
				y +=1
			x += 1
		y = first_height2
		while y<self.height: # Last column
			if self.board[self.width-1][y]==0 and self.board[self.width-2][y]==1:
				well_sums += 1
				yy = y+1
				while yy<self.height and self.board[self.width-1][yy]==0:
					well_sums += 1
					yy += 1
			y += 1


                # The decision depends on the strategy
                
		if strategy==-1: # specially tuned controller
                    eval = lines_removed                    
                    for i in self.range_width:
                        eval += self.w[i] * self.column_height[i]
                        if i<self.width-1:
                            eval += self.w[i+self.width] * abs(self.column_height[i]-self.column_height[i+1])
                    i=2*self.width-1
                    eval +=   self.w[i] * landing_height \
                            + self.w[i+1] * eroded_piece_cells \
                            + self.w[i+2] * row_transitions \
                            + self.w[i+3] * column_transitions \
                            + self.w[i+4] * hole_number \
                            + self.w[i+5] * well_sums \
                            + self.w[i+6] * max_height \
                            + self.w[i+7] * hole_depths \
                            + self.w[i+8] * rows_with_holes \
                            + self.w[i+9] * diversity
			#
		elif strategy==13: # 1_4_9_16, 6-7 (RLC6)
			eval = lines_removed \
			       -1.085554e+01 *landing_height \
			       -3.886515e+00 *eroded_piece_cells \
			       -9.680571e+00 *row_transitions \
			       -4.664301e+01 *column_transitions \
			       +7.664386e-01 *hole_number \
			       -2.057535e+00 *well_sums \
			       -3.085896e+00 *hole_depths \
			       -6.451461e+00 *rows_with_holes					#

		if debug:
			print "landing_height=", landing_height, "eroded_piece_cells=",eroded_piece_cells, "dr=",row_transitions, "dc=", column_transitions,  "hole_number=",  hole_number, "well_sums=", well_sums, "hole_depth=", hole_depths, "rows_w/holes=", rows_with_holes
			print "diversity: ",diversity, "(-2->", div_dm2,", (-1->", div_dm1,", (0->", div_d0,", (1->", div_d1,", (2->", div_d2,")"
			print "eval=",eval

			
		# restore the board as it was before we tried the macro
		if lines_removed==0:
			self.put_piece_on_board(self.piecenb,x2,y2,orient2,0)
		else: # it is too complicated so we make a plain copy
			self.restore_board_from_buffer()  

		
		return eval
		

	def plan(self):
	        "Set the best macro action"
		if debug:
			print "Planning the next macro..."

		self.copy_board_to_buffer() # make a copy (used in evaluate_macro to restore the board)
		bestl=[]
		besteval=-999999999.9
		nbor=self.pieces[self.piecenb][0]
		index=0
		for l in self.macros[self.piecenb]:					
			if (self.forcetetris_macros[self.piecenb][index]) or (self.height-self.first_height>=forcetetris_height):
				evaluation = self.evaluate_macro(l)
				if evaluation > besteval:
					besteval = evaluation
					bestl=l
			index+=1
		self.current_macro = bestl
		self.current_macro_index = 0
		if verbose:
			print "Chosen macro=",
			actions=["left", "right", "rotate_left", "rotate_right", "nop", "drop"]
			for i in self.current_macro:
				print actions[i],
			print  
			
######################################################################################################"

	def offline_optimization(self):				
		weightfile="mdp"+str(self.mdp_count-1)+".dat"
                logfile="log_mdptetris_rlc_"+weightfile

                # creating an init file for cross entropy:
                initfile="init_"+weightfile
                f=open(initfile,"w")
                f.write("1\n")
                f.write("1\n")
                nb_features=2*self.width+9
                f.write(str(nb_features)+"\n")
                for i in range(self.width):
                    f.write("8 0 100\n")              # column heights
                for i in range(self.width-1):
                    f.write("9 0 100\n")              # column absolute heights difference
                f.write("1 0 100\n")                  # Bertsekas + Dellacherie's features 
                f.write("2 0 100\n")
                f.write("3 0 100\n")
                f.write("4 0 100\n")
                f.write("5 0 100\n")
                f.write("6 0 100\n")
                f.write("7 0 100\n")
                f.write("-1 0 100\n")                 # Original features        
                f.write("-2 0 100\n")
                f.write("-14 0 100\n")
                f.close()
                
                # launch the optimizations of the weights
                print "Let's look offline for an adhoc controller for this MDP (log in "+logfile+")"

		cmd = prog+" -pieces ./pieces4.dat -reward lines -scores "+str(int(round(self.rewards_for_lines[1]/self.number_of_lines[1])))+" "+str(int(round(self.rewards_for_lines[2]/self.number_of_lines[2])))+" "+str(int(round(self.rewards_for_lines[3]/self.number_of_lines[3])))+" "+str(int(round(self.rewards_for_lines[4]/self.number_of_lines[4])))+" -height "+str(self.height)+" -width "+str(self.width)+" -nb_vectors_generated 250 -time 50000 -time_after_update 1000000 -initial_features "+initfile+" -final_features "+weightfile+" -nb_episodes 100 -noise linear 20 3 -dist "+str(self.piecenb_stat[0])+" "+str(self.piecenb_stat[1])+" "+str(self.piecenb_stat[2])+" "+str(self.piecenb_stat[3])+" "+str(self.piecenb_stat[4])+" "+str(self.piecenb_stat[5])+" "+str(self.piecenb_stat[6])+" > "+logfile

                command(cmd)
                print "Let's get the weights from the file "+weightfile
                try:
                    f=open(weightfile,"r")
                    f.readline(), # We don't care about the first two lines
                    f.readline(),
                    n=int(f.readline().rsplit()[0])
                    print n,"weights:",
                    for i in range(n):
                        s=f.readline()
                        self.w.append(float((s.rsplit())[1]))
                        print self.w[i],
                    f.close()
                    print
                except:
                    print "I/O problem opening/reading !!!"
                    self.w=[0]*50 # security

################################ Functions for interacting with RLGlue ################################

	def agent_init(self,taskSpec):
		global forcetetris_height,strategy,reward_strategy,strategy_index
		
		if debug:
			print ">agent_init"
		self.action = Action()
		self.action_types = []
		(version,episodic,states,actions,reward) = taskSpec.split(':')
		if debug:
			print "version=",version,"episodic=",episodic,"states=",states,"actions=",actions,"reward=",reward
		(stateDim,stateTypes,stateRanges) = states.split('_',2)
		if debug:
			print "stateDim=",stateDim,"stateTypes=",stateTypes,"stateRanges=",stateRanges
		stateRanges2=stateRanges.split('_')		
		self.height,self.width=eval(stateRanges2[-2])[0],eval(stateRanges2[-1])[0]				
		print "* Tetris MDP",self.mdp_count," *   height=",self.height,"width=",self.width, "board size=",self.height*self.width
		sys.stdout.flush()

		self.mdp_count += 1
			
		self.episode_count = 0

		# initializations 
		self.range_width=range(self.width)
		self.range_height=range(self.height)		

		self.pieces2_init()
		self.pieces_xy_init()
		self.macros_init()
		
		self.rewards_for_lines=[0.0]*5
		self.number_of_lines=[0]*5
		self.heights=[0]*(self.height+4)

		self.rows_with_holes_list=[0]*self.height

		# take the first strategy and initialize the vector of rewards of each strategy
		strategy_index=0
		(strategy,forcetetris_height)=strategies[0]	
		reward_strategy=[0]*strategy_number				
		
		self.this_mdp_sum_rewards=0
		self.this_mdp_step_number=0

		self.column_height=[0]*self.width

		self.piecenb_stat=[0]*7
		self.piece_count=0

		if save_pieces>0:
			self.pieces_file=open("pieces","w")
			self.pieces_file.write(str(save_pieces)+"\n")

                self.w=[]
                #self.offline_optimization() # DEBUG!
	
	def agent_start(self, observation):						
	
		if debug:
			print ">agent_start"
		#print "New game started"
		
		self.sum_rewards=0
		self.step_number=1
		if (self.this_mdp_step_number>=Learning_time*strategy_number) or ((self.this_mdp_step_number+1)%Learning_time!=0):   # this is to avoid a stupid bug in the learning phase
			self.this_mdp_step_number += 1
		
		self.board=[]
		self.board2=[]
		self.first_height=self.height      # no bricks at the beginning
		for i in self.range_width:			
			self.board.append([0]*self.height)
			self.board2.append([0]*self.height)
			
		self.get_next_piece(observation) # get the first piece

		self.plan()  # plan the corresponding macro

		return self.do_action(observation) # begin applying the macro

	
	def agent_step(self, reward, observation):

		global forcetetris_height,strategy,strategy_index,reward_strategy,sum_expected
		
		if debug:
                    print ">agent_step"
		if verbose:
                    print "MDP step number = ",self.this_mdp_step_number
                    print "Immediate reward = ",reward
			
		self.sum_rewards += reward
		self.this_mdp_sum_rewards += reward
		self.step_number += 1
		self.this_mdp_step_number += 1

		# Learning phase
		if (self.this_mdp_step_number<=Learning_time*strategy_number):

			if ((self.this_mdp_step_number % Learning_time) > pre_learning):
				reward_strategy[strategy_index] += reward

			if ((self.this_mdp_step_number % Learning_time) == 0):
				print "Test of strategy",strategy_index," (eval=",strategy,",ft_h=",forcetetris_height,") => return=",reward_strategy[strategy_index]," r/s=",1.0*reward_strategy[strategy_index]/(Learning_time-pre_learning)
                                print "Current score experience: rtot=",self.rewards_for_lines," r#=",self.number_of_lines
				strategy_index += 1			
				if (strategy_index == strategy_number): # if it is the last strategy, find the best!
					max=0
					strategy_index=0
					for i in range(strategy_number):
						if reward_strategy[i]>max:
							max=reward_strategy[i]
							strategy_index=i
					print "Chosen strategy:",strategy_index," (eval=",strategies[strategy_index][0],",ft_h=",strategies[strategy_index][1], ")  => Expected Reward/step =",1.0*max/(Learning_time-pre_learning)
					sum_expected += 1.0*max/(Learning_time-pre_learning)
				strategy=strategies[strategy_index][0]
				forcetetris_height=strategies[strategy_index][1]
				sys.stdout.flush()
				if (strategy,self.w)==(-1,[]): # this is the first time
					self.offline_optimization()
				

		if (self.this_mdp_step_number==SUICIDE_TIME):
			print "It is time (",SUICIDE_TIME,") to HARA KIRI !!!"
			sys.stdout.flush()

		if verbose:
			print "episode=",self.episode_count, "step=",self.step_number, "sum_rewards(game)=",self.sum_rewards,"sum_rewards(mdp)=",self.this_mdp_sum_rewards
		else:
			if self.step_number % 500==0: # every 500 steps
				sys.stderr.write("%i %i %f  %i %f  %f           \r" % (self.episode_count,self.step_number,self.sum_rewards,self.this_mdp_step_number,self.this_mdp_sum_rewards,self.this_mdp_sum_rewards / self.this_mdp_step_number))
				sys.stderr.flush()				
		
		if self.need_new_piece==2:    # it takes two transitions to get a new state
			self.need_new_piece=1
			self.rewards_for_lines[self.last_nb_lines_removed] += reward # store stats about rewards

			if verbose:
				print "Waiting for the next piece..."
			#self.print_observation(observation)
			return(self.action)                        # do whatever at this step
		elif self.need_new_piece==1:
			self.get_next_piece(observation)		 # getting new piece
			if verbose:
				self.print_observation(observation)
			if (SUICIDE_TIME<=0) or (self.this_mdp_step_number <= SUICIDE_TIME):
				self.plan() # plan macro action when new piece
			else: # do nothing...
				self.current_macro = SUICIDE_MACRO
				self.current_macro_index = 0
		else:
			if verbose:
				self.print_observation(observation)              # just observe
		return self.do_action(observation)                       # do the action in the macro action
	
	def agent_end(self,reward):
            
		if debug:
			print ">agent_end"
                        print "Game Over State:"
                        self.print_board(self.board)
		
		# print "steps=",self.step_number,"rewards=",self.sum_rewards,"       \r"		
		self.episode_count += 1

		sys.stdout.flush()
		
	
	def agent_cleanup(self):
		if debug:
			print ">agent_cleanup"
		# print "steps=",self.step_number,"rewards=",self.sum_rewards,"(ended before game over)"

		# showing some stats for the current MDP 		
		#print "Heights distribution\n",self.heights, "\n[",		
		#tot=0
		#for i in self.heights:
		#	tot+=i
		#for i in self.heights:
		#	print float(i)/tot,
		#print "]  tot=",tot
		print "Pieces dropped doing lines: ",self.number_of_lines,
		#print "[",
		tot=0
		for i in self.number_of_lines:
			tot+=i
		#for i in self.number_of_lines:
		#	print float(i)/tot,
		#print"]",
		print "  tot=",tot
		print "Rewards: ",
		for i in range(5):
			if self.number_of_lines[i]!=0:
				print self.rewards_for_lines[i]/self.number_of_lines[i],
			else:
				print "infty",
		# print "\nstrategy=", strategy,"forcetetris=",forcetetris_height
		print "\nPiece stats=",self.piecenb_stat
		self.total_sum_rewards += self.this_mdp_sum_rewards
		print "Last episode=",self.episode_count
		print "Tot.ret. of the MDP:",self.this_mdp_sum_rewards,"  Rew/step:",self.this_mdp_sum_rewards / self.this_mdp_step_number,"   Tot.ret.:", self.total_sum_rewards,"  Tot.exp.:",sum_expected,"\n"		
		sys.stdout.flush()
		
	def agent_freeze(self):
		if debug:
			print ">agent_freeze"
	
	def agent_message(self,inMessage):
		if debug:
			print ">agent_message signal: ",inMessage
		return None
	


# for psyco speed-up
psyco.bind(TetrisAgent.agent_step)
