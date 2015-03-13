import wx

import os
import time
import subprocess



class MainWindow(wx.Frame):
    title = "Fractal Explorer"
    xwin = 750
    ywin=750
    fname = "params.dat"
    params =[]
    #index number, Name, tuples
    #0-2 Camera Location    x,y,z
    #3-5 Camera Point       x,y,z
    #6   Field of View
    #7   Detail Level
    #8   Max Iterations
    #9   Escape Time
    #10  Brightness
    spinners = []
    status = 0
    btn =[]
    bitmap = 0
    control = 0 #This is what holds the static bitmap image
    def __init__(self, parent, id):

        wx.Frame.__init__(self, parent, id, 'Window', size=(self.xwin,self.ywin))
        self.InitGUI()

        self.PopulateSpinners()
        self.UpdateBitmap("image.bmp")

    def PopulateSpinners (self):
        # Load parameters into spinners
        self.ReadParams()
        for i in range(3):
            self.spinners[i].SetValue(float(self.params[2].split()[i]))
            self.spinners[i+3].SetValue(float(self.params[4].split()[i]))
        self.spinners[6].SetValue(float(self.params[8]))
        self.spinners[7].SetValue(float(self.params[13]))
        self.spinners[8].SetValue(float(self.params[18].split()[0]))
        self.spinners[9].SetValue(float(self.params[18].split()[1]))
        self.spinners[10].SetValue(float(self.params[23]))

    def ReadParams(self):
        # Reads parameters in from the params.dat file. File name set in
        # MainWindow Object
        f = open(self.fname, "r")
        for content in f:
            self.params.append(content)
        f.close()
    def InitGUI (self):
        #Hanldes intialization of all GUI compoenents
        panel = wx.Panel(self, -1)
        self.SetBackgroundColour(wx.Colour(90, 90, 90))
        self.Centre()
        self.Show()
        self.status = self.CreateStatusBar()


        # menubar = wx.MenuBar() #May use this eventually

        font1 = wx.Font(
            12, wx.MODERN, wx.NORMAL, wx.NORMAL, False, u'Consolas')
        font2 = wx.Font(
            16, wx.MODERN, wx.NORMAL, wx.NORMAL, False, u'Consolas')



        #---------------------------------------------
        #Generates spinners for camera position

        spinner_labels = []
        labels_names = ["Xpos", "Ypos", "Zpos"]
        spinner_pos = (self.xwin-100,40)
        spinner_size =(80,20)
        spinner_offset = 40
        for i, label in enumerate(labels_names):
            self.spinners.append(wx.SpinCtrlDouble(panel,\
                                            id=wx.ID_ANY,\
                                            value="0",\
                                            pos=(spinner_pos[0],spinner_pos[1]+i*spinner_offset),\
                                            size=spinner_size,style=wx.SP_ARROW_KEYS,\
                                            initial=0, inc=.5, min=-1000, max=1000))
            spinner_labels.append(wx.StaticText(panel, \
                                                -1,\
                                                labels_names[i], \
                                                (spinner_pos[0],spinner_pos[1]+i*spinner_offset-16)))
        Text2 = wx.StaticText(panel, -1, "Position",\
                                     (spinner_pos[0]-10,spinner_pos[1]-25))
        Text2.SetFont(font1)
        Text2.SetForegroundColour('white')
        #---------------------------------------------
        #Generates spinners for camera point direction

        spinner_labels = []
        spinner_pos = (self.xwin-200,40)
        for i, label in enumerate(labels_names):
            self.spinners.append(wx.SpinCtrlDouble(panel,\
                                            id=wx.ID_ANY,\
                                            value="0",\
                                            pos=(spinner_pos[0],spinner_pos[1]+i*spinner_offset),\
                                            size=spinner_size,style=wx.SP_ARROW_KEYS,\
                                            initial=0, inc=.5,min=-1000, max=1000))
            spinner_labels.append(wx.StaticText(panel, \
                                                -1,\
                                                labels_names[i], \
                                                (spinner_pos[0],spinner_pos[1]+i*spinner_offset-16)))
        Text1 = wx.StaticText(panel, -1, "Direction",\
                                     (spinner_pos[0],spinner_pos[1]-25))
        Text1.SetFont(font1)
        Text1.SetForegroundColour('white')
        #---------------------------------------------
        #Text for camera positions
        Text3 = wx.StaticText(panel, -1, "Camera Details",\
                                     (spinner_pos[0],spinner_pos[1]-40))
        Text3.SetFont(font2)
        Text3.SetForegroundColour('blue')
        #---------------------------------------------
        #Other parameters
        Text4 = wx.StaticText(panel, -1, "Mandel Parameters",\
                                     (spinner_pos[0],spinner_pos[1]+110))
        Text4.SetFont(font2)
        Text4.SetForegroundColour('blue')
        #----------------------------------------------
        #Other Parameter spinners
        spinner_labels = []
        labels_names = ["Field of View", "Detail Level", "Max Iterations", "Escape Time","Birghtness"]
        spinner_pos = (spinner_pos[0],spinner_pos[1]+140)
        spinner_size =(80,20)
        spinner_offset = 40
        for i, label in enumerate(labels_names):
            self.spinners.append(wx.SpinCtrlDouble(panel,\
                                            id=wx.ID_ANY,\
                                            value="0",\
                                            pos=(spinner_pos[0],spinner_pos[1]+i*spinner_offset),\
                                            size=spinner_size,style=wx.SP_ARROW_KEYS,\
                                            initial=0, inc=.5, min=-1000, max=1000))
            spinner_labels.append(wx.StaticText(panel, \
                                                -1,\
                                                labels_names[i], \
                                                (spinner_pos[0],spinner_pos[1]+i*spinner_offset-16)))

        #-----------------------------------------------
        #Set custom spinner increments
        #Some sinners require finer resolution. Set that here
        self.spinners[6].SetIncrement(.05)
        self.spinners[7].SetIncrement(.05)
        self.spinners[8].SetIncrement(1)
        self.spinners[9].SetIncrement(1)
        self.spinners[10].SetIncrement(.05)

        #-----------------------------------------------
        #Render button config

        xbutton_loc = self.xwin
        ybutton_loc = self.ywin
        buttx_size = 65
        butty_size = 20
        self.btn.append( wx.Button(panel,\
                        1,\
                        label='Render',\
                        pos=(xbutton_loc-buttx_size-10, ybutton_loc-80),\
                        size=(buttx_size, butty_size)))

        wx.EVT_BUTTON(self, 1, self.Render)
        self.btn[0].SetFont(wx.Font(14, wx.DEFAULT,wx.NORMAL, wx.NORMAL, 0, u'Consolas'))

        #If Enter is pushed call Render function
        panel.Bind(wx.EVT_KEY_DOWN, self.KeyPressed)

    def KeyPressed(self,event):
        keycode = event.GetKeyCode()
        if keycode == 13:
            self.Render()


    def Render (*arg):
        #Needed some hackery to get self into this function

        self = arg[0]

        #Wait until processing is done then update image
        #Try to update task bar while running

        arg[0].status.SetStatusText("Going to Render File",0)
        self.UpdateParams()


        #Write the new params file. This will involve copying the
        #current params file to a bak
        os.rename("params.dat", "params.dat.bak")
        fout = open("params.dat", "w")

        for line in self.params:
            fout.write(line)

        fout.close()

        #Run the .bmp generating c code


        subprocess.call('./mandelbox params.dat',shell=True)

        # time.sleep(20)
        os.remove("params.dat")

        #Puts the old params file back
        os.rename("params.dat.bak", "params.dat")
        self.UpdateBitmap("image.bmp")

    def UpdateParams(self):
        #Grabs all values from spinners and puts into param array
        #in preperation for writing to file for rendering


        #Camera position
        self.params[2]= str(self.spinners[0].GetValue())\
                 +" " + str(self.spinners[1].GetValue())\
                 +" " + str(self.spinners[2].GetValue()) + "\n"

        #Camera point direction
        self.params[4]= str(int(self.spinners[3].GetValue()))\
                 +" " + str(int(self.spinners[4].GetValue()))\
                 +" " + str(int(self.spinners[5].GetValue()))+ "\n"

        #Field of view
        self.params[8]= str(self.spinners[6].GetValue())+ "\n"
        #Detail level
        self.params[13]= str(self.spinners[7].GetValue())+ "\n"
        #Max iterations and escape time
        self.params[18]= str(int(self.spinners[8].GetValue()))\
                  +" " + str(int(self.spinners[9].GetValue()))+ "\n"
        #Brightness
        self.params[23]= str(self.spinners[10].GetValue())+ "\n"

    def UpdateBitmap (self, fname):
        self.bitmap = wx.Bitmap(fname)
        self.bitmap = self.scale_bitmap(500,500)
        control = wx.StaticBitmap(self, -1,self.bitmap)
        control.SetPosition((10,10))
        self.Update()

    def scale_bitmap (self,x,y):
        image = wx.ImageFromBitmap(self.bitmap)
        image = image.Scale(x,y, wx.IMAGE_QUALITY_HIGH)
        result = wx.BitmapFromImage(image)
        return result


if __name__ == '__main__':
    app = wx.PySimpleApp()
    frame = MainWindow(parent=None, id=-1)
    app.MainLoop()