/**
 * GetQueryResult.java
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

import java.io.FileNotFoundException;
import java.io.FileReader;
import java.io.IOException;
import java.io.PrintWriter;
import java.sql.PreparedStatement;
import java.sql.ResultSet;
import java.sql.SQLException;

import javax.servlet.ServletException;
import javax.servlet.annotation.WebServlet;
import javax.servlet.http.HttpServlet;
import javax.servlet.http.HttpServletRequest;
import javax.servlet.http.HttpServletResponse;

import org.json.JSONArray;
import org.json.JSONException;
import org.json.JSONObject;

import edu.ufl.cise.datapath.exception.DataPathError;
import edu.ufl.cise.datapath.utils.CSVReader;

/**
 * Servlet implementation class GetQueryResult
 */
@WebServlet ("/GetQueryResult")
public class GetQueryResult extends HttpServlet
{
    private static final long serialVersionUID = 1L;

    /**
     * @see HttpServlet#HttpServlet()
     */
    public GetQueryResult ()
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
        response.setHeader ("Access-Control-Allow-Origin", "*");
        response.setContentType ("application/json");
        connDB.connectToDB (DataPathConstants.DATAPATH_DB);
        PreparedStatement preStmt1 = null;
        ResultSet rs1 = null;
        int rezID = 0;
        JSONObject result = new JSONObject ();
        JSONObject cols = new JSONObject ();
        JSONArray rows = new JSONArray ();
        int length=0;
        try
        {
            rezID = Integer.parseInt (request.getParameter ("rez_id"));
            preStmt1 = connDB._conn.prepareStatement ("SELECT rezLocation FROM Query_Results WHERE rezID=?");
            preStmt1.setInt (1, rezID);
            rs1 = preStmt1.executeQuery ();
            if (rs1.next ())
            {
                CSVReader reader = new CSVReader(new FileReader(rs1.getString ("rezLocation")), ',');
                
                //put columns first (located in first two rows)
                String [] colNames = reader.readNext();
                String [] colTypes = reader.readNext();
                length = colNames.length;
                if(colNames != null && colTypes != null) {
                    for(int i=0; i<length; i++){
                        cols.put (""+i, new JSONObject ().put ("col_name", colNames[i]).put ("col_type", colTypes[i]));
                    }
                }

                String [] nextLine;
                while ((nextLine = reader.readNext()) != null) {
                    //nextLine[] is an array of values from the line
                    //"[{ \"v\": [" + hour + ", " + minute + ", " + second + ", 0]}, " + rand1 + "]"
                    String toPut = "[";
                    for(int i=0; i<length; i++){
                        if(i!=0){
                            toPut += ", ";    
                        }
                        
                        if(colTypes[i].equals ("string")){
                            toPut += "'" + nextLine[i]+ "'";
                        }
                        else if(colTypes[i].equals ("number")){
                            toPut += nextLine[i];                                                                        
                        }
                    }
                    toPut += "]";
                    rows.put (new JSONArray (toPut));                        
                }
            }
            
            result.put ("columns", cols)
                  .put ("col_count", length)
                  .put ("rows", rows);
            
            response.setStatus (DataPathConstants.STATUS_OK);
            out.println (result.toString ());
        } catch (SQLException e)
        {
            connDB.rollbackDB ();
            response.setStatus (DataPathConstants.INTERNAL_SERVER_ERROR);
            out.println (DataPathError.getErrorJson (DataPathConstants.INTERNAL_SERVER_ERROR, "SQLite Error: "+ e.getMessage ()));
        } catch (FileNotFoundException e)
        {
            response.setStatus (DataPathConstants.INTERNAL_SERVER_ERROR);
            out.println (DataPathError.getErrorJson (DataPathConstants.INTERNAL_SERVER_ERROR, "File Error: "+ e.getMessage ()));
        } catch (JSONException e)
        {
            response.setStatus (DataPathConstants.INTERNAL_SERVER_ERROR);
            out.print (DataPathError.getErrorJson (DataPathConstants.INTERNAL_SERVER_ERROR, "JSON Error: "+ e.getMessage ()));
        } finally
        {
            connDB.closePRC (preStmt1, rs1);
        }
    }
}
