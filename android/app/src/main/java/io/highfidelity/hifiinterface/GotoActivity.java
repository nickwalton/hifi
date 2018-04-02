package io.highfidelity.hifiinterface;

import android.app.ProgressDialog;
import android.content.Intent;
import android.graphics.Color;
import android.os.AsyncTask;
import android.os.Bundle;
import android.support.v4.view.GravityCompat;
import android.support.v4.widget.DrawerLayout;
import android.support.v7.app.ActionBar;
import android.support.v7.app.AppCompatActivity;
import android.support.v7.widget.GridLayoutManager;
import android.support.v7.widget.RecyclerView;
import android.support.v7.widget.Toolbar;
import android.view.Menu;
import android.view.MenuItem;
import android.view.View;
import android.widget.SearchView;
import android.widget.TabHost;
import android.widget.TabWidget;
import android.widget.TextView;

import io.highfidelity.hifiinterface.QtPreloader.QtPreloader;
import io.highfidelity.hifiinterface.view.DomainAdapter;

public class GotoActivity extends AppCompatActivity {

    /**
     * Set this intent extra param to NOT start a new InterfaceActivity after a domain is selected"
     */
    public static final String PARAM_NOT_START_INTERFACE_ACTIVITY = "not_start_interface_activity";
    private DomainAdapter domainAdapter;
    private DrawerLayout mDrawerLayout;
    private ProgressDialog mDialog;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_goto);
        Toolbar toolbar = (Toolbar) findViewById(R.id.toolbar);
        toolbar.setTitleTextAppearance(this, R.style.GotoActionBarTitleStyle);
        setSupportActionBar(toolbar);

        ActionBar actionbar = getSupportActionBar();
        actionbar.setDisplayHomeAsUpEnabled(true);
        actionbar.setHomeAsUpIndicator(R.drawable.ic_menu);

        mDrawerLayout = findViewById(R.id.drawer_layout);

        TabHost tabs=(TabHost)findViewById(R.id.tabhost);
        tabs.setup();

        TabHost.TabSpec spec=tabs.newTabSpec("featured");
        spec.setContent(R.id.featured);
        spec.setIndicator(getString(R.string.featured));
        tabs.addTab(spec);

        spec=tabs.newTabSpec("popular");
        spec.setContent(R.id.popular);
        spec.setIndicator(getString(R.string.popular));
        tabs.addTab(spec);

        spec=tabs.newTabSpec("bookmarks");
        spec.setContent(R.id.bookmarks);
        spec.setIndicator(getString(R.string.bookmarks));
        tabs.addTab(spec);

        tabs.setCurrentTab(0);

        TabWidget tabwidget=tabs.getTabWidget();
        for(int i=0;i<tabwidget.getChildCount();i++){
            TextView tv=(TextView) tabwidget.getChildAt(i).findViewById(android.R.id.title);
            tv.setTextAppearance(R.style.TabText);
        }


        RecyclerView domainsView = findViewById(R.id.rvDomains);
        int numberOfColumns = 1;
        GridLayoutManager gridLayoutMgr = new GridLayoutManager(this, numberOfColumns);
        domainsView.setLayoutManager(gridLayoutMgr);
        domainAdapter = new DomainAdapter(this);
        domainAdapter.setClickListener(new DomainAdapter.ItemClickListener() {

            @Override
            public void onItemClick(View view, int position, DomainAdapter.Domain domain) {
                Intent intent = new Intent(GotoActivity.this, InterfaceActivity.class);
                intent.putExtra(InterfaceActivity.DOMAIN_URL, domain.url);
                GotoActivity.this.finish();
                if (getIntent() != null &&
                    getIntent().hasExtra(PARAM_NOT_START_INTERFACE_ACTIVITY) &&
                    getIntent().getBooleanExtra(PARAM_NOT_START_INTERFACE_ACTIVITY, false)) {
                    intent.addFlags(Intent.FLAG_ACTIVITY_SINGLE_TOP);
                }
                startActivity(intent);
            }
        });
        domainsView.setAdapter(domainAdapter);

        SearchView searchView = findViewById(R.id.searchView);
        int searchPlateId = searchView.getContext().getResources().getIdentifier("android:id/search_plate", null, null);
        View searchPlate = searchView.findViewById(searchPlateId);
        if (searchPlate!=null) {
            searchPlate.setBackgroundColor (Color.TRANSPARENT);
            int searchTextId = searchPlate.getContext ().getResources ().getIdentifier ("android:id/search_src_text", null, null);
            TextView searchTextView = searchView.findViewById(searchTextId);
            searchTextView.setTextAppearance(R.style.SearchText);
        }

        preloadQt();

        if (getIntent() == null ||
                !getIntent().hasExtra(PARAM_NOT_START_INTERFACE_ACTIVITY) ||
                !getIntent().getBooleanExtra(PARAM_NOT_START_INTERFACE_ACTIVITY, false)) {
            preloadQt();
            showActivityIndicator();
        }

    }

    private void showActivityIndicator() {
        if (mDialog == null) {
            mDialog = new ProgressDialog(this);
        }
        mDialog.setMessage("Please wait...");
        mDialog.setCancelable(false);
        mDialog.show();
    }

    private void cancelActivityIndicator() {
        if (mDialog != null) {
            mDialog.cancel();
        }
    }

    private AsyncTask preloadTask;

    private void preloadQt() {
        if (preloadTask == null) {
            preloadTask = new AsyncTask() {
                @Override
                protected Object doInBackground(Object[] objects) {
                    new QtPreloader(GotoActivity.this).initQt();
                    runOnUiThread(new Runnable() {
                        @Override
                        public void run() {
                            cancelActivityIndicator();
                        }
                    });
                    return null;
                }
            };
            preloadTask.execute();
        }
    }

    @Override
    public boolean onCreateOptionsMenu(Menu menu) {
        // Inflate the menu; this adds items to the action bar if it is present.
        //getMenuInflater().inflate(R.menu.menu_goto, menu);
        return true;
    }

    @Override
    public boolean onOptionsItemSelected(MenuItem item) {
        // Handle action bar item clicks here. The action bar will
        // automatically handle clicks on the Home/Up button, so long
        // as you specify a parent activity in AndroidManifest.xml.
        int id = item.getItemId();
        switch (id) {
            case android.R.id.home:
                mDrawerLayout.openDrawer(GravityCompat.START);
                return true;
            case R.id.action_settings:
                return true;
        }

        return super.onOptionsItemSelected(item);
    }
}
