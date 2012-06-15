/**
 * GetChartData.java
 *
 * @author Praveen Salitra (praveen@cise.ufl.edu)
 *
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
 * COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 * BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
 * ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *
 */
package edu.ufl.cise.datapath;

import java.io.IOException;
import java.io.PrintWriter;
import java.sql.PreparedStatement;
import java.sql.ResultSet;
import java.sql.SQLException;
import java.util.Calendar;
import java.util.GregorianCalendar;

import javax.servlet.ServletException;
import javax.servlet.annotation.WebServlet;
import javax.servlet.http.HttpServlet;
import javax.servlet.http.HttpServletRequest;
import javax.servlet.http.HttpServletResponse;

import org.json.JSONArray;
import org.json.JSONException;
import org.json.JSONObject;

import edu.ufl.cise.datapath.exception.DataPathError;

/**
 * Servlet implementation class GetDPConfig
 */
@WebServlet ("/GetChartData")
public class GetChartData extends HttpServlet
{
    private static final long serialVersionUID = 1L;

    /**
     * @see HttpServlet#HttpServlet()
     */
    public GetChartData ()
    {
        super ();
    }

    /**
     * @see HttpServlet#doGet(HttpServletRequest request, HttpServletResponse
     *      response)
     */
    @Override
    protected void doGet (HttpServletRequest request, HttpServletResponse response) throws ServletException, IOException
    {
        doPost (request, response);
    }

    /**
     * @see HttpServlet#doPost(HttpServletRequest request, HttpServletResponse
     *      response)
     */
    @Override
    protected void doPost (HttpServletRequest request, HttpServletResponse response) throws ServletException, IOException
    {
        PrintWriter out = response.getWriter ();
        ConnectDB connDB = new ConnectDB ();
        response.setContentType ("application/json");
        response.setHeader ("Access-Control-Allow-Origin", "*");
        connDB.connectToDB (DataPathConstants.DATAPATH_DB);
        PreparedStatement preStmt1 = null;
        ResultSet rs1 = null;
        
        try
        {

            JSONObject jsonObj = new JSONObject();
            JSONObject graphObj = new JSONObject();
            JSONObject gaugeObj = new JSONObject();
        
            Calendar calendar = new GregorianCalendar();
            int hour = calendar.get(Calendar.HOUR);
            int minute = calendar.get(Calendar.MINUTE);
            int second = calendar.get(Calendar.SECOND);
            
            int rand1 = (int) (Math.random ()*100);
            graphObj.put ("cpu", new JSONArray ("[{ \"v\": [" + hour + ", " + minute + ", " + second + ", 0]}, " + rand1 + "]"));
            
            rand1 = (int) (Math.random ()*100);
            graphObj.put ("memory", new JSONArray ("[{ \"v\": [" + hour + ", " + minute + ", " + second + ", 0]}, " + rand1 + "]"));
            
            rand1 = (int) (Math.random ()*100);
            graphObj.put ("disk", new JSONArray ("[{ \"v\": [" + hour + ", " + minute + ", " + second + ", 0]}, " + rand1 + "]"));
            
            rand1 = (int) (Math.random ()*100);
            graphObj.put ("nw", new JSONArray ("[{ \"v\": [" + hour + ", " + minute + ", " + second + ", 0]}, " + rand1 + "]"));
            
            
            rand1 = (int) (Math.random ()*100);
            gaugeObj.put ("io", new JSONArray ("[{ \"v\": [" + hour + ", " + minute + ", " + second + ", 0]}, " + rand1 + "]"));
            rand1 = (int) (Math.random ()*100);
            gaugeObj.put ("chunk", new JSONArray ("[{ \"v\": [" + hour + ", " + minute + ", " + second + ", 0]}, " + rand1 + "]"));
            rand1 = (int) (Math.random ()*100);
            gaugeObj.put ("a", new JSONArray ("[{ \"v\": [" + hour + ", " + minute + ", " + second + ", 0]}, " + rand1 + "]"));
            rand1 = (int) (Math.random ()*100);
            gaugeObj.put ("b", new JSONArray ("[{ \"v\": [" + hour + ", " + minute + ", " + second + ", 0]}, " + rand1 + "]"));
            rand1 = (int) (Math.random ()*100);
            gaugeObj.put ("c", new JSONArray ("[{ \"v\": [" + hour + ", " + minute + ", " + second + ", 0]}, " + rand1 + "]"));
            rand1 = (int) (Math.random ()*100);
            gaugeObj.put ("d", new JSONArray ("[{ \"v\": [" + hour + ", " + minute + ", " + second + ", 0]}, " + rand1 + "]"));
            rand1 = (int) (Math.random ()*100);
            gaugeObj.put ("e", new JSONArray ("[{ \"v\": [" + hour + ", " + minute + ", " + second + ", 0]}, " + rand1 + "]"));
            rand1 = (int) (Math.random ()*100);
            gaugeObj.put ("f", new JSONArray ("[{ \"v\": [" + hour + ", " + minute + ", " + second + ", 0]}, " + rand1 + "]"));
            

            jsonObj.put ("graphs", graphObj)
                   .put ("gauges", gaugeObj);
            
            response.setStatus (DataPathConstants.STATUS_OK);
            out.print (jsonObj.toString ());

//        } catch (SQLException e)
//        {
//            connDB.rollbackDB ();
//            response.setStatus (DataPathConstants.INTERNAL_SERVER_ERROR);
//            out.print (DataPathError.getErrorJson (DataPathConstants.INTERNAL_SERVER_ERROR, "SQLite Error: "+ e.getMessage ()));
        } catch (JSONException e)
        {
            response.setStatus (DataPathConstants.INTERNAL_SERVER_ERROR);
            out.print (DataPathError.getErrorJson (DataPathConstants.INTERNAL_SERVER_ERROR, "JSON Error: "+ e.getMessage ()));
        } finally
        {
//            connDB.closePR (preStmt1, rs1);
//            connDB.closePR (preStmt2, rs2);
//            connDB.closePR (preStmt3, rs3);
//            connDB.closePR (preStmt4, rs4);            
//            connDB.closePRC (preStmt5, rs5);
        }
    }
}
